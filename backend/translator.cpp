#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <ctype.h>
#include <cstdint>

#include "common.h"
#include "ir.h"
#include "translator.h"
#include "dsl.h"

#define COMMA ,

FILE* fp = fopen("asmfile.asm", "w");
FILE* dump = fopen("dump.txt", "w");

size_t ALIGN4096 = 4096;
size_t BUFFERSIZE = 4096;

int TranslateIR(IR* ir, BinTrans* trans)
{
    trans->membuff = (unsigned char*) aligned_alloc(ALIGN4096, BUFFERSIZE);
    trans->labelstable.capacity = CountLabels(ir);
    trans->labelstable.size = 0;
    trans->labelstable.labels = (Label*) calloc(trans->labelstable.capacity, sizeof(Label));
    trans->exebuff = (unsigned char*) aligned_alloc(ALIGN4096, BUFFERSIZE);


    DBG fprintf(fp, "section .text\n");
    DBG fprintf(fp, "global main\n");

    DBG ExternFunctions();

    DBG fprintf(fp, "main:\n");

    DBG fprintf(fp, "mov r11, Buffer\n");

    Operation movr11 = {MOV_R11_NUM, SIZE_MOV_R11_NUM};
    WriteBinCmd(trans, &movr11);
    WriteAbsPtr(trans, (uint64_t) trans->membuff);

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
        TranslateFunc(&ir->functions[i], trans);
    }

    DBG fprintf(fp, "section .data\n"

                "Buffer: times 4096 db 0x00\n");
    DBG fprintf(fp, "formatin: db \"%%d\", 0x00\n");
    DBG fprintf(fp, "formatout: db \"%%d\", 0x0a, 0x00\n");
    DBG fprintf(fp, "Message: db \"HelloWorld!\", 0x0a, 0x00");

    DumpBuffer(trans);

    fclose(fp);
    return NOERR;
}

int TranslateFunc(FuncIR* function, BinTrans* trans)
{
    if (strcmp(function->name, "main") != 0)
    {
        DBG fprintf(fp, "%s\n", function->name);
    }
    EnlargeR11(function, trans);
    for (int i = 0; i < function->blocksnum; i++)
    {
        TranslateBlock(function, &function->blocks[i], trans);
    }
    if (strcmp(function->name, "main") == 0)
    {
        DBG fprintf(fp, "mov rax, 0x3c\n"
            "xor rdi, rdi\n"
            "syscall\n");
    }

    return NOERR;
}

int TranslateBlock(FuncIR* function, BlockIR* block, BinTrans* trans)
{
    if (block->size == 0)
    {
        fprintf(fp, ".%s\n", block->name);
        return NOERR;
    }

    if (block->commands[0].type != OP_ELSE)
    {
        DBG fprintf(fp, ".%s\n", block->name);
    }
    else
    {
        DBG fprintf(fp, "jmp .%s\n", block->commands[0].oper2.block->name);
        DBG fprintf(fp, ".%s\n", block->name);
    }

    for (int i = 0; i < block->size; i++)
    {
        TranslateCommand(function, block, &block->commands[i], trans);
    }

    return NOERR;
}

int TranslateCommand(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    switch (cmd->type)
    {
        case OP_VAR:
            TranslateVar(function, block, cmd, trans);
            break;
        case OP_EQ:
            TranslateEq(function, block, cmd, trans);
            break;
        case OP_ADD:
            TranslateArithOper(function, block, cmd, trans);
            break;
        case OP_SUB:
            TranslateArithOper(function, block, cmd, trans);
            break;
        case OP_MUL:
            TranslateArithOper(function, block, cmd, trans);
            break;
        case OP_DIV:
            TranslateArithOper(function, block, cmd, trans);
            break;
        case OP_CALL:
            TranslateCall(function, block, cmd, trans);
            break;
        case OP_RET:
            TranslateRet(function, block, cmd, trans);
            break;
        case OP_IF:
            TranslateIf(function, block, cmd, trans);
            break;
        case OP_PARAMIN:
            TranslateParamIn(function, block, cmd, trans);
            break;
        case OP_PARAMOUT:
            TranslateParamOut(function, block, cmd, trans);
            break;
        case OP_PARAMS:
            TranslateParamS(function, block, cmd, trans);
            break;
    }
    DumpBuffer(trans);

    return NOERR;
}

int TranslateVar(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
        {
            DBG fprintf(fp, "mov rax, %d\n", cmd->oper1.value);

            WriteMovRegNum(RAX, cmd->oper1.value);

            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
        case VARTYPE:
        {
            DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->oper1.var->offset);

            WriteMovRegMem(RAX, cmd->oper1.var->offset;

            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
    }
    return NOERR;
}

int TranslateEq(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
        {
            DBG fprintf(fp, "mov rax, %d\n", cmd->oper1.value);

            WriteMovRegNum(RAX, cmd->oper1.value);

            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
        case VARTYPE:
        {
            DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->oper1.var->offset);

            WriteMovRegMem(RAX, cmd->oper1.var->offset);

            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
    }

    return NOERR;
}

int TranslateArithOper(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    switch(cmd->oper1.type)
    {
        case NUMTYPE:
        {
            DBG fprintf(fp, "mov rax, %d\n", cmd->oper1.value);

            WriteMovRegNum(RAX, cmd->oper1.value);

            break;
        }
        case VARTYPE:
        {
            DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->oper1.var->offset);

            WriteMovRegMem(RAX, cmd->oper1.var->offset);

            break;
        }
    }
    switch(cmd->oper2.type)
    {
        case NUMTYPE:
        {
            DBG fprintf(fp, "mov rbx, %d\n", cmd->oper2.value);

            break;
        }
        case VARTYPE:
        {
            DBG fprintf(fp, "mov rbx, [r11 + %d]\n", cmd->oper2.var->offset);

            Operation movrbxmem = {}

            break;
        }
    }

    switch (cmd->type)
    {
        case OP_ADD:
        {
            DBG fprintf(fp, "add rax, rbx\n");
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
        case OP_SUB:
        {
            DBG fprintf(fp, "sub rax, rbx\n");
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
        case OP_MUL:
        {
            DBG fprintf(fp, "mul rbx\n");
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
        case OP_DIV:
        {
            DBG fprintf(fp, "div rbx\n");
            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
    }
    return NOERR;
}

int TranslateCall(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    if (strcmp(cmd->oper1.var->name, "zhmurik") == 0)
        TranslatePrintf(function, block, cmd, trans);
    else if (strcmp(cmd->oper1.var->name, "topdek") == 0)
        TranslateScanf(function, block, cmd, trans);
    else
    {
        DBG fprintf(fp, "call %s\n", cmd->oper1.var->name);
    }

    DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

    WriteMovMemReg(RAX, cmd->dest.var->offset);

    return NOERR;
}

int TranslateRet(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{

    switch(cmd->dest.type)
    {
        case NUMTYPE:
        {
            DBG fprintf(fp, "mov rax, %d\n", cmd->dest.value);

            WriteMovRegNum(RAX, cmd->dest.value);

            break;
        }
        case VARTYPE:
        {
            DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->dest.var->offset);

            WriteMovRegMem(RAX, cmd->dest.var->offset);

            break;
        }
    }

    ReduceR11(function, trans);
    DBG fprintf(fp, "ret\n");

    return NOERR;
}

int TranslateIf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
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

int TranslatePrintf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
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

int TranslateScanf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
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

int TranslateParamIn(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->dest.var->offset);

    WriteMovRegMem(RAX, cmd->dest.var->offset);

    DBG fprintf(fp, "push rax\n");

    WritePushReg(RAX);

    return NOERR;
}

int TranslateParamOut(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    DBG fprintf(fp, "pop rbx\n");

    WritePopReg(RBX);

    DBG fprintf(fp, "pop rax\n");

    WritePopReg(RAX);

    DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);

    WriteMovMemReg(RAX, cmd->dest.var->offset);

    DBG fprintf(fp,  "push rbx\n");

    WritePushReg(RBX);

    return NOERR;
}

int TranslateParamS(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    DBG fprintf(fp, "mov rax, r11\n");

    Operation movraxr11 = {MOV_RAX_R11, SIZE_MOV_RAX_R11};
    WriteBinCmd(trans, &movraxr11);

    DBG fprintf(fp, "add rax, %d\n", cmd->dest.var->offset);

    Operation addraxnum = {ADD_RAX_NUM, SIZE_ADD_RAX_NUM};
    WriteBinCmd(trans, &addraxnum);
    WriteNum(trans, cmd->dest.var->offset);

    DBG fprintf(fp, "push rax\n");

    WritePushReg(RAX);

    return NOERR;
}

int EnlargeR11(FuncIR* function, BinTrans* trans)
{
    DBG fprintf(fp, "add r11, %d\n", function->table.capacity * 8);

    Operation addr11num = {ADD_R11_NUM, SIZE_ADD_R11_NUM};
    WriteBinCmd(trans, &addr11num);
    WriteNum(trans, function->table.capacity * 8);

    return NOERR;
}

int ReduceR11(FuncIR* function, BinTrans* trans)
{
    DBG fprintf(fp, "sub r11, %d\n", function->table.capacity * 8);

    Operation subr11num = {SUB_R11_NUM, SIZE_SUB_R11_NUM};
    WriteBinCmd(trans, &subr11num);
    WriteNum(trans, function->table.capacity * 8);

    return NOERR;
}

int ExternFunctions()
{
    DBG fprintf(fp, "extern scanf\n");
    DBG fprintf(fp, "extern printf\n");

    return NOERR;
}

int CountLabels(IR* ir)
{
    int labels = 0;

    labels += ir->funcnum;

    for (int i = 0; i < ir->funcnum; i++)
    {
        labels += ir->functions[i].blocksnum;
    }

    return labels;
}

int WriteBinCmd(BinTrans* trans, Operation* oper)
{
    *(unsigned int*)(trans->exebuff + trans->len) = oper->opcode;
    trans->len += oper->size;

    return NOERR;
}

int WriteAbsPtr(BinTrans* trans, uint64_t ptr)
{
    *(uint64_t*) (trans->exebuff + trans->len) = ptr;
    trans->len += sizeof(uint64_t);

    return NOERR;
}

int WriteNum(BinTrans* trans, int num)
{
    *(int*) (trans->exebuff + trans->len) = num;
    trans->len += sizeof(int);

    return NOERR;
}

int DumpBuffer(BinTrans* trans)
{
    for (int i = 0; i < trans->len; i++)
    {
        fprintf(dump, "%02x", trans->exebuff[i]);
    }
    fprintf(dump, "\n");

    return NOERR;
}
