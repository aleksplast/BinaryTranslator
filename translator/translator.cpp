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

