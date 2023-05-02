#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "../functions/Config.h"
#include "../functions/text-sort.h"
#include "../functions/common.h"
#include "../functions/diftree.h"
#include "../functions/stack.h"

enum Location
{
    STACK,
    MEM,
    REG,
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

struct IRCommand
{
    OperType type;
    Variable dest;
    Variable oper1;
    Variable oper2;
    size_t size;
};

struct FuncIR
{
    VarTable table;
    IRCommand* commands;
    size_t size;
};

FuncIR* TreeToIR(Tree* tree);

int CountFunctions(Tree* tree);

int FuncToIR(Node* node, FuncIR* function);

int CountVariables(Node* node, int* num);

int FillVarTable(Node* node, VarTable* vartable, int* num);

VarTable CreateVarTable(Node* node);

Variable* SearchInTable(VarTable* vartable, int num, char* input);

#endif //BACKEND_H
