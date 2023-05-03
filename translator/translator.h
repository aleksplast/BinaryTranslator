#ifndef TRANSLATOR_H
#define TRANSLATOR_H

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
    IRCommand* commands;
    int numcommands;
    int capacity;
    size_t size;
};

struct FuncIR
{
    VarTable table;
    BlockIR* blocks;
    int blocksnum;
    size_t size;
};

FuncIR* TreeToIR(Tree* tree);

int CountFunctions(Tree* tree);

int FuncToIR(Node* node, FuncIR* function);

int CountVariables(Node* node, int* num);

int FillVarTable(Node* node, VarTable* vartable, int* num);

int NodeToIR(Node* node, FuncIR* function, int blocknum);

int BlockToIR(Node* node, FuncIR* function, int blocknum);

int CountBlocks(Node* node, int* num);

int CountCommands(Node* node, int* num);

int DumpFunc(FuncIR* function);

int DumpTable(FILE* file, VarTable* table);

int DumpVariable(FILE* fp, Variable* var);

int PrintLocation(FILE* fp, Location loc);

int DumpOperator(FILE* fp, OperatorIR* oper);

int DumpCommand(FILE* fp, IRCommand* command);

int DumpBlock(FILE* fp, BlockIR* block);

int PrintType(FILE* fp, OPERTYPE type);

VarTable CreateVarTable(Node* node);

Variable* SearchInTable(VarTable* vartable, int num, char* input);

#endif //BACKEND_H
