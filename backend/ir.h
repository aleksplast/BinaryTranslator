#ifndef IR_H
#define IR_H

#include "../functions/Config.h"
#include "../functions/text-sort.h"
#include "../functions/common.h"
#include "../functions/diftree.h"
#include "../functions/stack.h"

struct BlockIR;

enum Location
{
    STACK = 1,
    MEM,
    REG,
};

enum OPERTYPE
{
    NONE    = 0,
    VARTYPE = 1,
    BLOCKTYPE,
    NUMTYPE,
};

struct Variable
{
    char* name;
    Location location;
    int num;
    int offset;
};

struct VarTable
{
    Variable* table;
    int size;
    int capacity;
};

struct OperatorIR
{
    OPERTYPE type;
    Variable* var;
    int value;
    BlockIR* block;
};

struct IRCommand
{
    OperType type;
    OperatorIR dest;
    OperatorIR oper1;
    OperatorIR oper2;
    size_t size;
};

struct BlockIR
{
    char* name;
    IRCommand* commands;
    VarTable* table;
    int capacity;
    int size;
};

struct FuncIR
{
    VarTable table;
    BlockIR* blocks;
    int blocksnum;
    int curblock;
    char* name;
    size_t size;
};

struct IR
{
    FuncIR* functions;
    int funcnum;
};

FuncIR* TreeToIR(Tree* tree, IR* ir);

int CountFunctions(Tree* tree);

int FuncToIR(Node* node, FuncIR* function);

int CountVariables(Node* node, int* num);

int FillVarTable(Node* node, VarTable* vartable, int* num);

void* NodeToIR(Node* node,FuncIR* function,  BlockIR* block);

BlockIR* BlockToIR(Node* node, FuncIR* function, BlockIR* block);

void* VarToIR(Node* node,FuncIR* function,  BlockIR* block);

void* StatementToIR(Node* node,FuncIR* function,  BlockIR* block);

void* ArithOperToIR(Node* node,FuncIR* function,  BlockIR* block, OperType optype);

void* EqToIR(Node* node, FuncIR* function, BlockIR* block);

void* IfToIR(Node* node, FuncIR* function, BlockIR* block);

void* ElseToIR(Node* node, FuncIR* function, BlockIR* block);

void* CallToIR(Node* node, FuncIR* function, BlockIR* block);

void* ParamToIR(Node* node, FuncIR* function, BlockIR* block);

void* FunctionToIR(Node* node, FuncIR* function, BlockIR* block);

void* RetToIR(Node* node, FuncIR* function, BlockIR* block);

int AddOperator(Node* node, FuncIR* function, BlockIR* block, OperatorIR* oper);

int CountBlocks(Node* node, int* num);

int CountCommands(Node* node, int* num);

int DumpIR(IR* ir);

int DumpFunc(FILE* fp, FuncIR* function);

int DumpTable(FILE* file, VarTable* table);

int DumpVariable(FILE* fp, Variable* var);

int PrintLocation(FILE* fp, Location loc);

int DumpOperator(FILE* fp, OperatorIR* oper);

int DumpCommand(FILE* fp, IRCommand* command);

int DumpBlock(FILE* fp, BlockIR* block);

int PrintType(FILE* fp, OPERTYPE type);

VarTable CreateVarTable(Node* node);

Variable* SearchInTable(VarTable* vartable, int num, char* input);

int IrDtor(IR* ir);

int FuncDtor(FuncIR* func);

int BlockDtor(BlockIR* block);

int TableDtor(VarTable* table);

#endif //IR_H
