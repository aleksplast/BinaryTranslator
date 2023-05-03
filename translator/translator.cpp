#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <ctype.h>

#include "common.h"
#include "../translator/translator.h"

#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch"


static OperatorIR fake = {NONE, NULL, 0, NULL};
int VARSIZE = 40;

FuncIR* TreeToIR(Tree* tree)
{
    int funcnum = CountFunctions(tree);
    Node* node = tree->anchor;
    int counter = 0;
    printf("numfunc = %d\n", funcnum);

    FuncIR* functions = (FuncIR*) calloc(funcnum, sizeof(FuncIR));

    while(node != NULL)
    {
        if (node->leftchild->optype == OP_FUNC)
            FuncToIR(node->leftchild, &functions[counter]);
        counter += 1;
        node = node->rightchild;
    }

    return 0;
}

int CountFunctions(Tree* tree)
{
    Node* node = tree->anchor;
    int numfunc = 0;

    while (node != NULL)
    {
        if (node->leftchild->optype == OP_FUNC)
            numfunc += 1;
        node = node->rightchild;
    }

    return numfunc;
}

int FuncToIR(Node* node, FuncIR* function)
{
    VarTable vartable = CreateVarTable(node);
    function->table = vartable;

    int blocksnum = 1;
    CountBlocks(node, &blocksnum);
    function->blocksnum = blocksnum;
    printf("blocksnum = %d\n", blocksnum);
    function->blocks = (BlockIR*) calloc(function->blocksnum, sizeof(BlockIR));

    printf("HERE1\n");
    BlockToIR(node, function, 0);
    DumpFunc(function);

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
    if (node->type == OP_TYPE && node->optype != OP_STAT && node->optype != OP_NIL && node->optype != OP_FUNC)
        *num += 1;
    if (node->leftchild)
    {
        if (node->leftchild->optype == OP_IF || node->leftchild->optype == OP_WHILE || node->leftchild->optype == OP_ELSE)
            return NOERR;
    }
    if (node->rightchild)
        CountCommands(node->rightchild, num);
    if (node->leftchild)
        CountCommands(node->leftchild, num);

    return NOERR;
}

int BlockToIR(Node* node, FuncIR* function, int blocknum)
{
    int numcommands = 0;
    CountCommands(node, &numcommands);
    printf("numcommands = %d\n", numcommands);
    function->blocks[blocknum].numcommands = numcommands;
    function->blocks[blocknum].commands = (IRCommand*) calloc(numcommands, sizeof(IRCommand));

    printf("HERE2\n");
    NodeToIR(node, function, blocknum);

    return NOERR;
}

int NodeToIR(Node* node, FuncIR* function, int blocknum)
{
    printf("HERE\n");
    switch(node->optype)
    {
        case OP_VAR:
            printf("capacity = %d\n", function->blocks[blocknum].capacity);
            function->blocks[blocknum].commands[function->blocks[blocknum].capacity].type = OP_VAR;
            Variable* var = SearchInTable(&function->table, function->table.size, node->leftchild->varvalue);
            function->blocks[blocknum].commands[function->blocks[blocknum].capacity].dest.type = VARTYPE;
            function->blocks[blocknum].commands[function->blocks[blocknum].capacity].dest.var = var;
            function->blocks[blocknum].commands[function->blocks[blocknum].capacity].oper1.type = NUMTYPE;
            function->blocks[blocknum].commands[function->blocks[blocknum].capacity].oper1.value = node->rightchild->val;
            function->blocks[blocknum].commands[function->blocks[blocknum].capacity].oper2 = fake;
            function->blocks[blocknum].capacity += 1;
    }
    if (node->leftchild)
        NodeToIR(node->leftchild, function, blocknum);
    if (node->rightchild)
        NodeToIR(node->rightchild, function, blocknum);
    if (node->type != OP_TYPE || node->optype == OP_STAT || node->optype == OP_NIL || node->optype == OP_FUNC)
        return NOERR;

    return NOERR;
}

VarTable CreateVarTable(Node* node)
{
    VarTable vartable = {};

    int varnum = 0;
    CountVariables(node, &varnum);
    vartable.size = varnum;

    vartable.table = (Variable*) calloc(varnum, sizeof(Variable));

    varnum = 0;
    FillVarTable(node, &vartable, &varnum);
    vartable.size = varnum;

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
    if (node->type == OP_TYPE && node->optype <= OP_DIV)
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
    }
    if (node->type == OP_TYPE && node->optype <= OP_DIV)
    {
        char* name = (char*) calloc(VARSIZE, sizeof(char));
        sprintf(name, "temp%d", *num);

        vartable->table[*num].name = name;
        vartable->table[*num].location = STACK;
        vartable->table[*num].num = *num;
        vartable->table[*num].offset = (*num + 1) * 8;
        *num += 1;
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
        if (strcasecmp(vartable->table[i].name, input) == 0)
            return &vartable->table[i];
    }

    return NULL;
}

int DumpFunc(FuncIR* function)
{
    FILE* fp = fopen("dumpfile.txt", "a");

    printf("dumping\n");
    DumpTable(fp, &function->table);

    for (int i = 0; i < function->blocksnum; i++)
    {
        fprintf(fp, "Dumping Block%d\n", i);
        fprintf(fp, "-------------------------\n");
        DumpBlock(fp, &function->blocks[i]);
        fprintf(fp, "-------------------------\n");
    }

    fclose(fp);

    return NOERR;
}

int DumpBlock(FILE* fp, BlockIR* block)
{
    fprintf(fp, "\tNumber of Commands: %d\n", block->numcommands);
    fprintf(fp, "\tSize = %ld\n\n", block->size);
    for (int i = 0; i < block->numcommands; i++)
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
            fprintf(fp, "\tBlock = %p\n", oper->block);
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
    printf("Dumping variable\n");
    fprintf(fp, "name = %s, num = %d, location = ", var->name, var->num);
    PrintLocation(fp, var->location);
    fprintf(fp, ", offset = %d\n", var->offset);

    return NOERR;
}

int DumpTable(FILE* fp, VarTable* table)
{
    fprintf(fp, "Dumping VarTable\n");
    fprintf(fp, "-------------------------\n");
    fprintf(fp, "VarTable size = %d\n", table->size);
    for (int i = 0; i < table->size; i++)
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

