#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <ctype.h>

#include "common.h"
#include "ir.h"
#include "translator.h"

FILE* fp = fopen("asmfile.asm", "w");

int TranslateIR(IR* ir)
{
    fprintf(fp, "section .text\n");
    fprintf(fp, "global _start\n");
    fprintf(fp, "_start:\n");

    for(int i = 0; i < ir->funcnum; i++)
    {
        TranslateFunc(&ir->functions[i]);
        fprintf(fp, "section .data\n"

                "%sBuffer times 256 db 0\n", ir->functions[i].name);
        fprintf(fp, "section .text\n");
    }


    fclose(fp);
    return NOERR;
}

int TranslateFunc(FuncIR* function)
{
    fprintf(fp, "%s\n", function->name);
    for (int i = 0; i < function->blocksnum; i++)
    {
        TranslateBlock(function, &function->blocks[i]);
    }
    if (strcmp(function->name, "main") == 0)
    {
        fprintf(fp, "mov rax, 0x3c\n"
            "xor rdi, rdi\n"
            "syscall\n");
    }

    return NOERR;
}

int TranslateBlock(FuncIR* function, BlockIR* block)
{
    if (block->size == 0)
    {
        fprintf(fp, ".%s\n", block->name);
        return NOERR;
    }

    if (block->commands[0].type != OP_ELSE)
        fprintf(fp, ".%s\n", block->name);
    else
    {
        fprintf(fp, "jmp .%s\n", block->commands[0].oper2.block->name);
        fprintf(fp, ".%s\n", block->name);
    }

    for (int i = 0; i < block->size; i++)
    {
        TranslateCommand(function, block, &block->commands[i]);
    }
}

int TranslateCommand(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch (cmd->type)
    {
        case OP_VAR:
            TranslateVar(function, block, cmd);
            break;
        case OP_EQ:
            TranslateEq(function, block, cmd);
            break;
        case OP_ADD:
            TranslateArithOper(function, block, cmd);
            break;
        case OP_SUB:
            TranslateArithOper(function, block, cmd);
            break;
        case OP_MUL:
            TranslateArithOper(function, block, cmd);
            break;
        case OP_DIV:
            TranslateArithOper(function, block, cmd);
            break;
        case OP_CALL:
            TranslateCall(function, block, cmd);
            break;
        case OP_RET:
            TranslateRet(function, block, cmd);
            break;
        case OP_IF:
            TranslateIf(function, block, cmd);
            break;
    }

    return NOERR;
}

int TranslateVar(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
            fprintf(fp, "mov rax, %d\n", cmd->oper1.value);
            fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name, cmd->dest.var->offset);
            break;
        case VARTYPE:
            fprintf(fp, "mov rax, [%sBuffer + %d]\n", function->name, cmd->oper1.var->offset);
            fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name, cmd->dest.var->offset);
            break;
    }
    return NOERR;
}

int TranslateEq(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
            fprintf(fp, "mov rax, %d\n", cmd->oper1.value);
            fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name, cmd->dest.var->offset);
            break;
        case VARTYPE:
            fprintf(fp, "mov rax, [%sBuffer + %d]\n", function->name, cmd->oper1.var->offset);
            fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name, cmd->dest.var->offset);
            break;
    }

    return NOERR;
}

int TranslateArithOper(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
            fprintf(fp, "mov rax, %d\n", cmd->oper1.value);
            break;
        case VARTYPE:
            fprintf(fp, "mov rax, [%sBuffer + %d]\n", function->name,cmd->oper1.var->offset);
            break;
    }
    switch(cmd->oper2.type)
    {
        case NUMTYPE:
            fprintf(fp, "mov rbx, %d\n", cmd->oper1.value);
            break;
        case VARTYPE:
            fprintf(fp, "mov rbx, [%sBuffer + %d]\n", function->name,cmd->oper1.var->offset);
            break;
    }

    switch (cmd->type)
    {
        case OP_ADD:
            fprintf(fp, "add rax, rbx\n");
            fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name, cmd->dest.var->offset);
            break;
        case OP_SUB:
            fprintf(fp, "sub rax, rbx\n");
            fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name, cmd->dest.var->offset);
            break;
        case OP_MUL:
            fprintf(fp, "mul rbx\n");
            fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name, cmd->dest.var->offset);
            break;
        case OP_DIV:
            fprintf(fp, "dib rbx\n");
            fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name,cmd->dest.var->offset);
            break;
    }
    return NOERR;
}

int TranslateCall(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    fprintf(fp, "call %s\n", cmd->oper1.var->name);

    fprintf(fp, "mov [%sBuffer + %d], rax\n", function->name,cmd->dest.var->offset);

    return NOERR;
}

int TranslateRet(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->dest.type)
    {
        case NUMTYPE:
           fprintf(fp, "mov rax, %d\n", cmd->dest.value);
           fprintf(fp, "ret\n");
           break;
        case VARTYPE:
           fprintf(fp, "mov rax, [%sBuffer + %d]\n", function->name,cmd->dest.var->offset);
           fprintf(fp, "ret\n");
           break;
    }

    return NOERR;
}

int TranslateIf(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->dest.type)
    {
        case NUMTYPE:
            fprintf(fp, "cmp %d, 0\n", cmd->dest.value);
            break;
        case VARTYPE:
            fprintf(fp, "cmp [%sBuffer + %d], 0\n", function->name, cmd->dest.var->offset);
            break;
    }

    fprintf(fp, "je .%s\n", cmd->oper1.block->name);
    fprintf(fp, "jmp .%s\n", cmd->oper2.block->name);

    return NOERR;
}
