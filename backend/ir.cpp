#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <ctype.h>

#include "common.h"
#include "../backend/ir.h"

#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch"

#define CurCmd block->commands[block->size]
#define NewCmd function->blocks[function->curblock].commands[function->blocks[function->curblock].size]
#define PrevCmd (void*)&block->table->table[block->table->size - 1]

static int ifcount = 0;

static OperatorIR fake = {NONE, NULL, 0, NULL};
int VARSIZE = 40;
int CurBlock = 0;

FuncIR* TreeToIR(Tree* tree, IR* ir)
{
    int funcnum = CountFunctions(tree);
    ir->funcnum = funcnum;

    Node* node = tree->anchor;
    int counter = 0;

    ir->functions = (FuncIR*) calloc(funcnum, sizeof(FuncIR));

    while(node != NULL)
    {
        if (node->leftchild->optype == OP_FUNC && node->leftchild->rightchild != NULL)
        {
            FuncToIR(node->leftchild, &ir->functions[counter]);
            counter += 1;
        }
        node = node->rightchild;
    }

    DumpIR(ir);

    return 0;
}

int CountFunctions(Tree* tree)
{
    Node* node = tree->anchor;
    int numfunc = 0;

    while (node != NULL)
    {
        if (node->leftchild->optype == OP_FUNC && node->leftchild->rightchild != NULL)
            numfunc += 1;
        node = node->rightchild;
    }

    return numfunc;
}

int FuncToIR(Node* node, FuncIR* function)
{
    FILE* table = fopen("vartable.txt", "a");
    VarTable vartable = CreateVarTable(node);
    DumpTable(table, &vartable);
    fclose(table);

    function->table = vartable;

    function->name = node->leftchild->varvalue;

    int blocksnum = 1;
    CountBlocks(node, &blocksnum);
    function->blocksnum = blocksnum;
    function->blocks = (BlockIR*) calloc(function->blocksnum + 1, sizeof(BlockIR));
    function->curblock = 0;

    for (int i = 0; i < function->blocksnum; i++)
    {
        char* blockname = (char*) calloc(VARSIZE, sizeof(char));
        sprintf(blockname, "block%d", i);

        function->blocks[i].name = blockname;
    }

    BlockToIR(node, function, &function->blocks[0]);

    return NOERR;
}

int CountBlocks(Node* node, int* num)
{
    if (node->optype == OP_WHILE || node->optype == OP_IF || node->optype == OP_ELSE)
        *num += 2;
    if (node->rightchild)
        CountBlocks(node->rightchild, num);
    if (node->leftchild)
        CountBlocks(node->leftchild, num);

    return NOERR;
}

int CountCommands(Node* node, int* num)
{
    if (node->type == OP_TYPE && node->optype != OP_STAT && node->optype != OP_NIL && node->optype != OP_FUNC && node->optype != OP_ELSE)
        *num += 1;
    if (node->leftchild)
    {
        if (node->leftchild->optype == OP_IF || node->leftchild->optype == OP_WHILE)
        {
            *num += 1;
            CountCommands(node->leftchild->leftchild, num);
            return NOERR;
        }
    }
    if (node->optype == OP_ELSE)
        return CountCommands(node->leftchild, num);
    if (node->rightchild)
        CountCommands(node->rightchild, num);
    if (node->leftchild)
        CountCommands(node->leftchild, num);

    return NOERR;
}

BlockIR* BlockToIR(Node* node, FuncIR* function, BlockIR* block)
{
    int numcommands = 0;
    CountCommands(node, &numcommands);

    if (node->ancestor)
    {
        if (node->ancestor->optype == OP_ELSE)
        {
            numcommands += 1;
        }
    }

    block->capacity = numcommands;

    block->commands = (IRCommand*) calloc(numcommands, sizeof(IRCommand));
    block->table = &function->table;

    if (node->ancestor)
    {
        if (node->ancestor->optype == OP_ELSE)
        {

            CurCmd.type = OP_ELSE;
            CurCmd.oper1.type = BLOCKTYPE;
            CurCmd.oper2.type = BLOCKTYPE;
            CurCmd.oper1.block = block;
            CurCmd.oper2.block = &function->blocks[function->curblock + 1];
            block->size += 1;
        }
    }

    NodeToIR(node, function, block);

    return block;
}

void* NodeToIR(Node* node, FuncIR* function, BlockIR* block)
{
    switch(node->optype)
    {
        case OP_VAR:
            return VarToIR(node, function, block);
        case OP_EQ:
            return EqToIR(node, function, block);
        case OP_ADD:
            return ArithOperToIR(node, function, block, OP_ADD);
        case OP_MUL:
            return ArithOperToIR(node, function, block, OP_MUL);
        case OP_DIV:
            return ArithOperToIR(node, function, block, OP_DIV);
        case OP_SUB:
            return ArithOperToIR(node, function, block, OP_SUB);
        case OP_STAT:
            return StatementToIR(node, function, block);
        case OP_IF:
            return IfToIR(node, function, block);
        case OP_ELSE:
            return ElseToIR(node, function, block);
        case OP_CALL:
            return CallToIR(node, function, block);
        case OP_PARAM:
            return ParamToIR(node, function, block);
        case OP_FUNC:
            return FunctionToIR(node, function, block);
        case OP_RET:
            return RetToIR(node, function, block);
        case OP_NIL:
            return NULL;
    }

    return NULL;
}

void* RetToIR(Node* node, FuncIR* function, BlockIR* block)
{
    Variable* oper1 = NULL;
    OPERTYPE type1 = NONE;
    int val1 = 0;

    if (node->leftchild->type == OP_TYPE)
    {
        type1 = VARTYPE;
        oper1 = (Variable*) NodeToIR(node->leftchild, function, block);
    }
    else if (node->leftchild->type == NUM_TYPE)
    {
        type1 = NUMTYPE;
        val1 = node->leftchild->val;
    }
    else
    {
        type1 = VARTYPE;
        oper1 = SearchInTable(block->table, block->table->capacity, node->leftchild->varvalue);
    }

    CurCmd.type = OP_RET;

    CurCmd.dest.type = type1;
    CurCmd.dest.value = val1;
    CurCmd.dest.var = oper1;

    block->size += 1;

    return PrevCmd;
}

void* FunctionToIR(Node* node, FuncIR* function, BlockIR* block)
{
    if (node->leftchild->leftchild)
        NodeToIR(node->leftchild->leftchild, function, block);
    if (node->rightchild)
        NodeToIR(node->rightchild, function, block);
    return NULL;
}

void* StatementToIR(Node* node, FuncIR* function, BlockIR* block)
{
    if (node->leftchild->optype == OP_IF)
    {
        NodeToIR(node->leftchild, function, block);
        function->curblock += 1;
        if (node->rightchild)
            BlockToIR(node->rightchild, function, &function->blocks[function->curblock]);
        return NULL;
    }
    if (node->leftchild)
        NodeToIR(node->leftchild, function, block);
    if (node->rightchild)
        NodeToIR(node->rightchild, function, block);

    return NULL;
}

void* ArithOperToIR(Node* node, FuncIR* function, BlockIR* block, OperType optype)
{
    Variable* oper1 = NULL;
    Variable* oper2 = NULL;
    OPERTYPE type1 = NONE;
    OPERTYPE type2 = NONE;
    int val1 = 0;
    int val2 = 0;

    if (node->rightchild->type == OP_TYPE)
    {
        oper2 = (Variable*) NodeToIR(node->rightchild, function, block);
        type2 = VARTYPE;
    }
    else if (node->rightchild->type == VAR_TYPE)
    {
        oper2 = SearchInTable(block->table, block->table->capacity, node->rightchild->varvalue);
        type2 = VARTYPE;
    }
    else
    {
        type2 = NUMTYPE;
        val2 = node->rightchild->val;
    }

    if (node->leftchild->type == OP_TYPE)
    {
        oper1 = (Variable*) NodeToIR(node->leftchild, function, block);
        type1 = VARTYPE;
    }
    else if (node->leftchild->type == VAR_TYPE)
    {
        oper1 = SearchInTable(block->table, block->table->capacity, node->leftchild->varvalue);
        type1 = VARTYPE;
    }
    else
    {
        type1 = NUMTYPE;
        val1 = node->leftchild->val;
    }

    CurCmd.oper1.type = type1;
    CurCmd.oper1.var = oper1;
    CurCmd.oper1.value = val1;
    CurCmd.oper2.type = type2;
    CurCmd.oper2.var = oper2;
    CurCmd.oper2.value = val2;

    CurCmd.type = optype;

    char* temp = (char*) calloc(VARSIZE, sizeof(char));
    sprintf(temp, "temp%d", block->table->size);

    block->table->table[block->table->size].location = STACK;
    block->table->table[block->table->size].name = temp;
    block->table->table[block->table->size].offset = (block->table->size + 1) * 8;
    block->table->table[block->table->size].num = block->table->size;

    CurCmd.dest.type = VARTYPE;
    CurCmd.dest.var = &block->table->table[block->table->size];

    block->table->size += 1;
    block->size += 1;

    return PrevCmd;
}

void* EqToIR(Node* node, FuncIR* function, BlockIR* block)
{
    Variable* oper1 = NULL;
    OPERTYPE type1 = NONE;
    int val1 = 0;

    if (node->rightchild->type == OP_TYPE)
    {
        type1 = VARTYPE;
        oper1 = (Variable*) NodeToIR(node->rightchild, function, block);
    }
    else if (node->rightchild->type == NUM_TYPE)
    {
        type1 = NUMTYPE;
        val1 = node->rightchild->val;
    }
    else
    {
        type1 = VARTYPE;
        oper1 = SearchInTable(block->table, block->table->capacity, node->rightchild->varvalue);
    }

    CurCmd.type = OP_EQ;
    CurCmd.dest.type = VARTYPE;
    CurCmd.dest.var = SearchInTable(block->table, block->table->capacity, node->leftchild->varvalue);

    CurCmd.oper1.type = type1;
    CurCmd.oper1.value = val1;
    CurCmd.oper1.var = oper1;

    block->size += 1;

    return PrevCmd;
}

void* VarToIR(Node* node, FuncIR* function, BlockIR* block)
{
    Variable* oper1 = NULL;
    OPERTYPE type1 = NONE;
    int val1 = 0;

    if (node->rightchild == NULL)
    {;}
    else if (node->rightchild->type == OP_TYPE)
    {
        type1 = VARTYPE;
        oper1 = (Variable*) NodeToIR(node->rightchild, function, block);
    }
    else if (node->rightchild->type == NUM_TYPE)
    {
        type1 = NUMTYPE;
        val1 = node->rightchild->val;
    }
    else if (node->rightchild)
    {
        type1 = VARTYPE;
        oper1 = SearchInTable(block->table, block->table->capacity, node->rightchild->varvalue);
    }

    CurCmd.type = OP_VAR;
    CurCmd.dest.type = VARTYPE;
    CurCmd.dest.var = SearchInTable(block->table, block->table->capacity, node->leftchild->varvalue);

    CurCmd.oper1.type = type1;
    CurCmd.oper1.value = val1;
    CurCmd.oper1.var = oper1;

    block->size += 1;

    return PrevCmd;
}

void* IfToIR(Node* node, FuncIR* function, BlockIR* block)
{
    Variable* oper1 = NULL;
    OPERTYPE type1 = NONE;
    int val1 = 0;

    if (node->leftchild->type == OP_TYPE)
    {
        type1 = VARTYPE;
        oper1 = (Variable*) NodeToIR(node->leftchild, function, block);
    }
    else if (node->leftchild->type == NUM_TYPE)
    {
        type1 = NUMTYPE;
        val1 = node->leftchild->val;
    }
    else
    {
        type1 = VARTYPE;
        oper1 = SearchInTable(block->table, block->table->capacity, node->leftchild->varvalue);
    }

    CurCmd.type = OP_IF;

    CurCmd.dest.type = type1;
    CurCmd.dest.value = val1;
    CurCmd.dest.var = oper1;
    function->curblock += 1;

    CurCmd.oper1.type = BLOCKTYPE;
    CurCmd.oper1.block = BlockToIR(node->rightchild, function, &function->blocks[function->curblock]);

    CurCmd.oper2.type = BLOCKTYPE;
    if (node->rightchild->optype == OP_ELSE)
        CurCmd.oper2.block = &function->blocks[function->curblock];
    else
        CurCmd.oper2.block = &function->blocks[function->curblock + 1];
    block->size += 1;

    return PrevCmd;
}

void* ElseToIR(Node* node, FuncIR* function, BlockIR* block)
{
    if (node->leftchild)
    {
        NodeToIR(node->leftchild, function, block);
        function->curblock += 1;
    }
    if (node->rightchild)
    {
        BlockToIR(node->rightchild, function, &function->blocks[function->curblock]);
    }

    return NULL;
}

void* CallToIR(Node* node, FuncIR* function, BlockIR* block)
{
    if (node->leftchild->leftchild)
        NodeToIR(node->leftchild->leftchild, function, block);

    CurCmd.type = OP_CALL;
    CurCmd.oper1.type = VARTYPE;
    CurCmd.oper1.var = SearchInTable(block->table, block->table->capacity, node->leftchild->varvalue);

    char* temp = (char*) calloc(VARSIZE, sizeof(char));
    sprintf(temp, "temp%d", block->table->size);

    block->table->table[block->table->size].location = STACK;
    block->table->table[block->table->size].name = temp;
    block->table->table[block->table->size].offset = (block->table->size + 1) * 8;
    block->table->table[block->table->size].num = block->table->size;

    CurCmd.dest.type = VARTYPE;
    CurCmd.dest.var = &block->table->table[block->table->size];

    block->table->size += 1;
    block->size += 1;

    return PrevCmd;
}

void* ParamToIR(Node* node, FuncIR* function, BlockIR* block)
{
    Variable* oper1 = NULL;
    OPERTYPE type1 = NONE;
    int val1 = 0;

    if (node->leftchild->optype == OP_VAR)
    {
        type1 = VARTYPE;
        oper1 = SearchInTable(block->table, block->table->capacity, node->leftchild->leftchild->varvalue);
    }
    else if (node->leftchild->type == OP_TYPE)
    {
        type1 = VARTYPE;
        oper1 = (Variable*) NodeToIR(node->leftchild, function, block);
    }
    else if (node->leftchild->type == NUM_TYPE)
    {
        type1 = NUMTYPE;
        val1 = node->leftchild->val;
    }
    else
    {
        type1 = VARTYPE;
        oper1 = SearchInTable(block->table, block->table->capacity, node->leftchild->varvalue);
    }

    if (node->leftchild->optype == OP_VAR)
        CurCmd.type = OP_PARAMOUT;
    else if (strcmp(node->ancestor->varvalue, "topdek") == 0)
        CurCmd.type = OP_PARAMS;
    else
        CurCmd.type = OP_PARAMIN;

    CurCmd.dest.type = type1;
    CurCmd.dest.value = val1;
    CurCmd.dest.var = oper1;

    block->size += 1;

    if (node->rightchild)
        NodeToIR(node->rightchild, function, block);

    return NULL;
}

VarTable CreateVarTable(Node* node)
{
    VarTable vartable = {};

    int varnum = 0;
    CountVariables(node, &varnum);
    vartable.capacity = varnum;

    vartable.table = (Variable*) calloc(varnum, sizeof(Variable));

    varnum = 0;
    vartable.capacity = 0;
    FillVarTable(node, &vartable, &varnum);


    return vartable;
}

int CountVariables(Node* node, int* num)
{
    if (node->leftchild)
        CountVariables(node->leftchild, num);
    if (node->rightchild)
        CountVariables(node->rightchild, num);
    if (node->type == VAR_TYPE && node->ancestor->optype != OP_FUNC)
        *num += 1;
    if (node->type == OP_TYPE && (node->optype <= OP_DIV || node->optype == OP_CALL))
        *num += 1;

    return NOERR;
}

int FillVarTable(Node* node, VarTable* vartable, int* num)
{
    if (node->type == VAR_TYPE && node->ancestor->optype != OP_FUNC)
    {
        if (SearchInTable(vartable, *num, node->varvalue))
            return NOERR;

        char* name = (char*) calloc(VARSIZE, sizeof(char));
        sprintf(name, "%s", node->varvalue);

        vartable->table[*num].name = name;
        vartable->table[*num].location = STACK;
        vartable->table[*num].num = *num;
        vartable->table[*num].offset = (*num + 1) * 8;

        *num += 1;
        vartable->capacity += 1;
        vartable->size += 1;
    }

    if (node->type == OP_TYPE && (node->optype <= OP_DIV || node->optype == OP_CALL))
    {
        vartable->capacity += 1;
    }

    if (node->leftchild)
        FillVarTable(node->leftchild, vartable, num);
    if (node->rightchild)
        FillVarTable(node->rightchild, vartable, num);

    return NOERR;
}

Variable* SearchInTable(VarTable* vartable, int num, char* input)
{
    for (int i = 0; i < num; i++)
    {
        if (vartable->table[i].name == NULL){;}
        else if (strcasecmp(vartable->table[i].name, input) == 0)
            return &vartable->table[i];
    }

    return NULL;
}

int DumpIR(IR* ir)
{
    FILE* fp = fopen("dumpfile.txt", "a");

    for (int i = 0; i < ir->funcnum; i++)
    {
        fprintf(fp, "Dumping function %s\n", ir->functions[i].name);
        fprintf(fp, "---------------------------------------------------------------\n");
        DumpFunc(fp, &ir->functions[i]);
        fprintf(fp, "---------------------------------------------------------------\n\n\n");
    }

    fclose(fp);

    return NOERR;
}

int DumpFunc(FILE* fp, FuncIR* function)
{
    DumpTable(fp, &function->table);

    for (int i = 0; i < function->blocksnum; i++)
    {
        fprintf(fp, "Dumping %s\n", function->blocks[i].name);
        fprintf(fp, "-------------------------\n");
        DumpBlock(fp, &function->blocks[i]);
        fprintf(fp, "-------------------------\n\n");
    }

    return NOERR;
}

int DumpBlock(FILE* fp, BlockIR* block)
{
    fprintf(fp, "\tNumber of Commands: %d\n", block->capacity);
    fprintf(fp, "\tSize = %d\n\n", block->size);
    for (int i = 0; i < block->capacity; i++)
    {
        fprintf(fp, "\tDumping command%d\n", i);
        fprintf(fp, "\t---------------\n");
        DumpCommand(fp, &block->commands[i]);
        fprintf(fp, "\t---------------\n\n");
    }

    return NOERR;
}

int DumpCommand(FILE* fp, IRCommand* command)
{
    fprintf(fp, "\t");
    OpTypePrint(fp, command->type);
    fprintf(fp, "\n\tDest:");
    DumpOperator(fp, &command->dest);
    fprintf(fp, "\tOper1:");
    DumpOperator(fp, &command->oper1);
    fprintf(fp, "\tOper2:");
    DumpOperator(fp, &command->oper2);

    return NOERR;
}

int DumpOperator(FILE* fp, OperatorIR* oper)
{
    PrintType(fp, oper->type);
    switch (oper->type)
    {
        case NUMTYPE:
            fprintf(fp, "\tval = %d\n", oper->value);
            break;
        case VARTYPE:
            DumpVariable(fp, oper->var);
            break;
        case BLOCKTYPE:
            fprintf(fp, "\tBlock = %s\n", oper->block->name);
            break;
        case NONE:
            fprintf(fp, " NONE \n");
    }

    return NOERR;
}

int PrintType(FILE* fp, OPERTYPE type)
{
    switch (type)
    {
        case NUMTYPE:
            fprintf(fp, " NumType ");
            break;
        case VARTYPE:
            fprintf(fp, " VarType ");
            break;
        case BLOCKTYPE:
            fprintf(fp, " BlockType ");
            break;
        case NONE:
            fprintf(fp, "");
            break;
    }

    return NOERR;
}

int DumpVariable(FILE* fp, Variable* var)
{
    fprintf(fp, "name = %s, num = %d, location = ", var->name, var->num);
    PrintLocation(fp, var->location);
    fprintf(fp, ", offset = %d\n", var->offset);

    return NOERR;
}

int DumpTable(FILE* fp, VarTable* table)
{
    fprintf(fp, "Dumping VarTable\n");
    fprintf(fp, "-------------------------\n");
    fprintf(fp, "VarTable size = %d\n", table->capacity);
    for (int i = 0; i < table->capacity; i++)
    {
        fprintf(fp, "Variable%d: name = %s, num = %d, location = ", i, table->table[i].name, table->table[i].num);
        PrintLocation(fp, table->table[i].location);
        fprintf(fp, ", offset = %d\n", table->table[i].offset);
    }
    fprintf(fp, "-------------------------\n\n");

    return NOERR;
}

int PrintLocation(FILE* fp, Location loc)
{
    switch (loc)
    {
        case STACK:
            fprintf(fp, "Stack");
            break;
        case MEM:
            fprintf(fp, "Memory");
            break;
        case REG:
            fprintf(fp, "Register");
            break;
        default:
            break;
    }

    return NOERR;
}

int IrDtor(IR* ir)
{
    for (int i = 0; i < ir->funcnum; i++)
    {
        FuncDtor(&ir->functions[i]);
    }

    free(ir->functions);
    ir->functions = NULL;
    ir->funcnum = -1;

    return NOERR;
}

int FuncDtor(FuncIR* func)
{
    for (int i = 0; i < func->blocksnum; i++)
    {
        BlockDtor(&func->blocks[i]);
    }

    TableDtor(&func->table);
    free(func->name);
    func->name = NULL;

    func->blocksnum = -1;
    func->curblock = -1;
    func->size = -1;

    free(func->blocks);
    func->blocks = NULL;

    return NOERR;
}

int BlockDtor(BlockIR* block)
{
    free(block->commands);
    block->commands = NULL;

    block->capacity = -1;
    block->size = -1;

    free(block->name);
    block->name = NULL;

    return NOERR;
}

int TableDtor(VarTable* table)
{
    for (int i = 0; i < table->size; i++)
    {
        free(table->table[i].name);
        table->table[i].name = NULL;
        table->table[i].num = -1;
        table->table[i].offset = -1;
    }

    free(table->table);
    table->table = NULL;

    table->size = -1;
    table->capacity = -1;
}
