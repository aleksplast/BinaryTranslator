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

size_t align4096 = 4096;
size_t buffersize = 4096;

int TranslateIR(IR* ir)
{
    char* buffer = (char*) aligned_alloc(align4096, buffersize);
    DBG fprintf(fp, "section .text\n");
    DBG fprintf(fp, "global main\n");

    DBG ExternFunctions();

    DBG fprintf(fp, "main:\n");
    DBG fprintf(fp, "mov r11, Buffer\n");

    DBG fprintf(fp, "push r11\n");
    DBG fprintf(fp, "push rbp\n");
    DBG fprintf(fp, "push rsp\n");
    DBG fprintf(fp, "mov rdi, Message\n");
    DBG fprintf(fp, "call printf\n");
    DBG fprintf(fp, "pop rsp\n");
    DBG fprintf(fp, "pop rbp\n");
    DBG fprintf(fp, "pop r11\n");

    for(int i = 0; i < ir->funcnum; i++)
    {
        TranslateFunc(&ir->functions[i]);
    }

    DBG fprintf(fp, "section .data\n"

                "Buffer: times 4096 db 0x00\n");
    DBG fprintf(fp, "formatin: db \"%%d\", 0x00\n");
    DBG fprintf(fp, "formatout: db \"%%d\", 0x00\n");
    DBG fprintf(fp, "Message: db \"HelloWorld!\", 0x0a, 0x00");


    fclose(fp);
    return NOERR;
}

int TranslateFunc(FuncIR* function)
{
    if (strcmp(function->name, "main") != 0)
        fprintf(fp, "%s\n", function->name);
    EnlargeR11(function);
    for (int i = 0; i < function->blocksnum; i++)
    {
        TranslateBlock(function, &function->blocks[i]);
    }
    if (strcmp(function->name, "main") == 0)
    {
        DBG fprintf(fp, "mov rax, 0x3c\n"
            "xor rdi, rdi\n"
            "syscall\n");
    }

    return NOERR;
}

int TranslateBlock(FuncIR* function, BlockIR* block)
{
    if (block->size == 0)
    {
        DBG fprintf(fp, ".%s\n", block->name);
        return NOERR;
    }

    if (block->commands[0].type != OP_ELSE)
        DBG fprintf(fp, ".%s\n", block->name);
    else
    {
        DBG fprintf(fp, "jmp .%s\n", block->commands[0].oper2.block->name);
        DBG fprintf(fp, ".%s\n", block->name);
    }

    for (int i = 0; i < block->size; i++)
    {
        TranslateCommand(function, block, &block->commands[i]);
    }

    return NOERR;
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
        case OP_PARAMIN:
            TranslateParamIn(function, block, cmd);
            break;
        case OP_PARAMOUT:
            TranslateParamOut(function, block, cmd);
            break;
        case OP_PARAMS:
            TranslateParamS(function, block, cmd);
            break;
    }

    return NOERR;
}

int TranslateVar(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
            DBG fprintf(fp, "mov rax, %d\n", cmd->oper1.value);
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            break;
        case VARTYPE:
            DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->oper1.var->offset);
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            break;
    }
    return NOERR;
}

int TranslateEq(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
            DBG fprintf(fp, "mov rax, %d\n", cmd->oper1.value);
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            break;
        case VARTYPE:
            DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->oper1.var->offset);
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            break;
    }

    return NOERR;
}

int TranslateArithOper(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
            DBG fprintf(fp, "mov rax, %d\n", cmd->oper1.value);
            break;
        case VARTYPE:
            DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->oper1.var->offset);
            break;
    }
    switch(cmd->oper2.type)
    {
        case NUMTYPE:
            DBG fprintf(fp, "mov rbx, %d\n", cmd->oper2.value);
            break;
        case VARTYPE:
            DBG fprintf(fp, "mov rbx, [r11 + %d]\n", cmd->oper2.var->offset);
            break;
    }

    switch (cmd->type)
    {
        case OP_ADD:
            DBG fprintf(fp, "add rax, rbx\n");
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            break;
        case OP_SUB:
            DBG fprintf(fp, "sub rax, rbx\n");
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            break;
        case OP_MUL:
            DBG fprintf(fp, "mul rbx\n");
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            break;
        case OP_DIV:
            DBG fprintf(fp, "div rbx\n");
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            break;
    }
    return NOERR;
}

int TranslateCall(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    if (strcmp(cmd->oper1.var->name, "zhmurik") == 0)
        TranslatePrintf(function, block, cmd);
    else if (strcmp(cmd->oper1.var->name, "topdek") == 0)
        TranslateScanf(function, block, cmd);
    else
    {
        DBG fprintf(fp, "call %s\n", cmd->oper1.var->name);
    }

    DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

    return NOERR;
}

int TranslateRet(FuncIR* function, BlockIR* block, IRCommand* cmd)
{

    switch(cmd->dest.type)
    {
        case NUMTYPE:
           DBG fprintf(fp, "mov rax, %d\n", cmd->dest.value);
           break;
        case VARTYPE:
           DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->dest.var->offset);
           break;
    }

    ReduceR11(function);
    DBG fprintf(fp, "ret\n");

    return NOERR;
}

int TranslateIf(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    switch(cmd->dest.type)
    {
        case NUMTYPE:
            DBG fprintf(fp, "cmp %d, 0\n", cmd->dest.value);
            break;
        case VARTYPE:
            DBG fprintf(fp, "cmp DWORD [r11 + %d], 0\n", cmd->dest.var->offset);
            break;
    }

    DBG fprintf(fp, "jne .%s\n", cmd->oper1.block->name);
    DBG fprintf(fp, "jmp .%s\n", cmd->oper2.block->name);

    return NOERR;
}

int TranslatePrintf(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    DBG fprintf(fp, "pop rax\n");
    DBG fprintf(fp, "push r11\n");
    DBG fprintf(fp, "push rbp\n");
    DBG fprintf(fp, "push rsp\n");
    DBG fprintf(fp, "mov rdi, formatout\n");
    DBG fprintf(fp, "mov rsi, rax\n");
    DBG fprintf(fp, "call printf\n");
    DBG fprintf(fp, "pop rsp\n");
    DBG fprintf(fp, "pop rbp\n");
    DBG fprintf(fp, "pop r11\n");

    return NOERR;
}

int TranslateScanf(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    DBG fprintf(fp, "pop rax\n");
    DBG fprintf(fp, "push r11\n");
    DBG fprintf(fp, "push rbp\n");
    DBG fprintf(fp, "push rsp\n");
    DBG fprintf(fp, "mov rdi, formatin\n");
    DBG fprintf(fp, "mov rsi, rax\n");
    DBG fprintf(fp, "call scanf\n");
    DBG fprintf(fp, "pop rsp\n");
    DBG fprintf(fp, "pop rbp\n");
    DBG fprintf(fp, "pop r11\n");

    return NOERR;
}

int TranslateParamIn(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->dest.var->offset);
    DBG fprintf(fp, "push rax\n");

    return NOERR;
}

int TranslateParamOut(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    DBG fprintf(fp, "pop rbx\n");
    DBG fprintf(fp, "pop rax\n");
    DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
    DBG fprintf(fp,  "push rbx\n");

    return NOERR;
}

int TranslateParamS(FuncIR* function, BlockIR* block, IRCommand* cmd)
{
    DBG fprintf(fp, "mov rax, r11\n");
    DBG fprintf(fp, "add rax, %d\n", cmd->dest.var->offset);
    DBG fprintf(fp, "push rax\n");

    return NOERR;
}

int EnlargeR11(FuncIR* function)
{
    DBG fprintf(fp, "add r11, %d\n", function->table.capacity * 8);

    return NOERR;
}

int ReduceR11(FuncIR* function)
{
    DBG fprintf(fp, "sub r11, %d\n", function->table.capacity * 8);

    return NOERR;
}

int ExternFunctions()
{
    DBG fprintf(fp, "extern scanf\n");
    DBG fprintf(fp, "extern printf\n");

    return NOERR;
}
