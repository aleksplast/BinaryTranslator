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
size_t STRSIZE = 100;
char INITIAL[40] = "INITIAL";
unsigned int ENTRY = 0x400080;
int PRINTSIZE = 139;

int BinTransCtor(IR* ir, BinTrans* trans)
{
    assert(ir != NULL);
    assert(trans != NULL);

    trans->membuff = (unsigned char*) aligned_alloc(ALIGN4096, BUFFERSIZE);
    trans->exebuff = (unsigned char*) aligned_alloc(ALIGN4096, BUFFERSIZE);

    trans->labelstable.capacity = CountLabels(ir);
    trans->labelstable.size = 0;
    trans->labelstable.labels = (Label*) calloc(trans->labelstable.capacity + 1, sizeof(Label));

    trans->formatout = (char*) calloc(STRSIZE + 1, sizeof(char));
    trans->formatin = (char*) calloc(STRSIZE + 1, sizeof(char));
//    sprintf(trans->formatout, "\"%%d\"\n");
//    sprintf(trans->formatin, "\"%%d\"");

    return NOERR;
}

int BinTransDtor(BinTrans* trans)
{
    assert(trans != NULL);

    free(trans->exebuff);
    trans->exebuff = NULL;

    free(trans->labelstable.labels);
    trans->labelstable.labels = NULL;

    trans->len = 1;
    trans->labelstable.capacity = -1;
    trans->labelstable.size = -1;

    return NOERR;
}

int TranslateIR(IR* ir, BinTrans* trans)
{
    assert(ir != NULL);
    assert(trans != NULL);

    trans->len = 0;
    DBG fprintf(fp, "section .text\n");
    DBG fprintf(fp, "global main\n");

    DBG ExternFunctions();

    DBG fprintf(fp, "main:\n");

    DBG fprintf(fp, "mov r11, Buffer\n");

    WriteCmd(MOV_R12_NUM);
    WriteNum(trans, ENTRY + 2 * BUFFERSIZE - 20);

    WriteCmd(MOV_R13_R12);

    WriteCmd(MOV_R11_BUF);
    WriteNum(trans, ENTRY + 2 * BUFFERSIZE);

    WriteCmd(MOV_R14_NUM);
    WriteNum(trans, ENTRY + 2 * BUFFERSIZE - 40);

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

    trans->printptr = ENTRY + BUFFERSIZE;

    DBG fprintf(fp, "section .data\n"

                "Buffer: times 4096 db 0x00\n");
    DBG fprintf(fp, "formatin: db \"%%d\", 0x00\n");
    DBG fprintf(fp, "formatout: db \"%%d\", 0x0a, 0x00\n");
    DBG fprintf(fp, "Message: db \"HelloWorld!\", 0x0a, 0x00");

    DBG DumpBuffer(trans);

    return NOERR;
}

int TranslateFunc(FuncIR* function, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);

    if (strcmp(function->name, "main") != 0)
    {
        DBG fprintf(fp, "%s\n", function->name);
        AddLabel(trans, function->name, INITIAL);
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
        WriteCmd(MOV_RAX_NUM);
        WriteNum(trans, 0x3c);

        WriteCmd(XOR_RDI_RDI);

        WriteCmd(SYSCALL);
    }

    return NOERR;
}

int TranslateBlock(FuncIR* function, BlockIR* block, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);

    if (block->size == 0)
    {
        fprintf(fp, ".%s\n", block->name);
        AddLabel(trans, function->name, block->name);
        return NOERR;
    }

    if (block->commands[0].type != OP_ELSE)
    {
        DBG fprintf(fp, ".%s\n", block->name);
        AddLabel(trans, function->name, block->name);
    }
    else
    {
        DBG fprintf(fp, "jmp .%s\n", block->commands[0].oper2.block->name);
        WriteUnCndJump(trans, function->name, block->commands[0].oper2.block->name);

        DBG fprintf(fp, ".%s\n", block->name);
        AddLabel(trans, function->name, block->name);
    }

    for (int i = 0; i < block->size; i++)
    {
        TranslateCommand(function, block, &block->commands[i], trans);
    }

    return NOERR;
}

int TranslateCommand(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

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
    DBG DumpBuffer(trans);

    return NOERR;
}

int TranslateVar(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

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

int TranslateEq(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

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
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

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
            WriteMovRegNum(RBX, cmd->oper2.value);

            break;
        }
        case VARTYPE:
        {
            DBG fprintf(fp, "mov rbx, [r11 + %d]\n", cmd->oper2.var->offset);
            WriteMovRegMem(RBX, cmd->oper2.var->offset);

            break;
        }
    }

    switch (cmd->type)
    {
        case OP_ADD:
        {
            DBG fprintf(fp, "add rax, rbx\n");
            WriteCmd(ADD_RAX_RBX);

            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
        case OP_SUB:
        {
            DBG fprintf(fp, "sub rax, rbx\n");
            WriteCmd(SUB_RAX_RBX);

            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
        case OP_MUL:
        {
            DBG fprintf(fp, "mul rbx\n");
            WriteCmd(MUL_RBX);

            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
        case OP_DIV:
        {
            DBG fprintf(fp, "div rbx\n");
            WriteCmd(DIV_RBX);

            DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
            WriteMovMemReg(RAX, cmd->dest.var->offset);

            break;
        }
    }
    return NOERR;
}

int TranslateCall(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

    if (strcmp(cmd->oper1.var->name, "zhmurik") == 0)
        TranslatePrintf(function, block, cmd, trans);
    else if (strcmp(cmd->oper1.var->name, "topdek") == 0)
        TranslateScanf(function, block, cmd, trans);
    else
    {
        DBG fprintf(fp, "call %s\n", cmd->oper1.var->name);
        WriteCall(trans, cmd->oper1.var->name);
    }

    DBG fprintf(fp, "mov [r11 + %d], rax\n", cmd->dest.var->offset);
    WriteMovMemReg(RAX, cmd->dest.var->offset);

    return NOERR;
}

int TranslateRet(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

    if (strcmp(function->name, "main") == 0)
        return NOERR;

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
    WriteCmd(RET);

    return NOERR;
}

int TranslateIf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

    switch(cmd->dest.type)
    {
        case NUMTYPE:
        {
            DBG fprintf(fp, "cmp %d, 0\n", cmd->dest.value);

            break;
        }
        case VARTYPE:
        {
            DBG fprintf(fp, "cmp DWORD [r11 + %d], 0\n", cmd->dest.var->offset);

            WriteCmd(CMP_MEM_R11_0);
            WriteNum(trans, cmd->dest.var->offset);
            WriteNum(trans, 0);

            break;
        }
    }

    DBG fprintf(fp, "jne .%s\n", cmd->oper1.block->name);
    WriteCndJump(trans, function->name, cmd->oper1.block->name);

    DBG fprintf(fp, "jmp .%s\n", cmd->oper2.block->name);
    WriteUnCndJump(trans, function->name, cmd->oper2.block->name);

    return NOERR;
}

int TranslatePrintf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

    DBG fprintf(fp, "pop rax\n");
    WritePopReg(RAX);

    DBG fprintf(fp, "push r11\n");
    WritePushReg(R11);

    DBG fprintf(fp, "push rbp\n");
    WritePushReg(RBP);

    DBG fprintf(fp, "push rsp\n");
    WritePushReg(RSP);
//
//     WritePushReg(R12);
//     WritePushReg(R13);

    DBG fprintf(fp, "mov rdi, formatout\n");
    // Operation movrdi = {MOV_RDI_NUM, SIZE_MOV_RDI_NUM};
    // WriteBinCmd(trans, &movrdi);
    // WriteAbsPtr(trans, (uint64_t)trans->formatout);

//     DBG fprintf(fp, "mov rsi, rax\n");
//     WriteCmd(MOV_RDI_RAX);
//
//     DBG fprintf(fp, "call printf\n");
//     WriteCmd(CALL);
//
//     uint32_t printptr = (uint64_t)Print - sizeof(int) - trans->len - (uint64_t)trans->exebuff;
//     WriteNum(trans, printptr);
    WriteCmd(CALL);
    WriteNum(trans, BUFFERSIZE - sizeof(int) - trans->len);

    // WritePopReg(R13);
    // WritePopReg(R12);
    // WriteCmd(MOV_R13_R12);

    DBG fprintf(fp, "pop rsp\n");
    WritePopReg(RSP);

    DBG fprintf(fp, "pop rbp\n");
    WritePopReg(RBP);

    DBG fprintf(fp, "pop r11\n");
    WritePopReg(R11);

    return NOERR;
}

int TranslateScanf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

    DBG fprintf(fp, "pop rax\n");
    WritePopReg(RAX);

    DBG fprintf(fp, "push r11\n");
    WritePushReg(R11);

    DBG fprintf(fp, "push rbp\n");
    WritePushReg(RBP);

    DBG fprintf(fp, "push rsp\n");
    WritePushReg(RSP);

    DBG fprintf(fp, "mov rdi, formatin\n");
    // Operation movrdi = {MOV_RDI_NUM, SIZE_MOV_RDI_NUM};
    // WriteBinCmd(trans, &movrdi);
    // WriteAbsPtr(trans, (uint64_t)trans->formatout);

    DBG fprintf(fp, "mov rsi, rax\n");
    WriteCmd(MOV_RDI_RAX);

    // DBG fprintf(fp, "call scanf\n");
    WriteCmd(CALL);
    WriteNum(trans, BUFFERSIZE + PRINTSIZE - sizeof(int) - trans->len);
//
//     uint32_t scanfptr = (uint64_t)Scanf - sizeof(int) - trans->len - (uint64_t)trans->exebuff;
//     WriteNum(trans, scanfptr);

    DBG fprintf(fp, "pop rsp\n");
    WritePopReg(RSP);

    DBG fprintf(fp, "pop rbp\n");
    WritePopReg(RBP);

    DBG fprintf(fp, "pop r11\n");
    WritePopReg(R11);

    return NOERR;
}

int TranslateParamIn(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

    DBG fprintf(fp, "mov rax, [r11 + %d]\n", cmd->dest.var->offset);
    WriteMovRegMem(RAX, cmd->dest.var->offset);

    DBG fprintf(fp, "push rax\n");
    WritePushReg(RAX);

    return NOERR;
}

int TranslateParamOut(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

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
    assert(function != NULL);
    assert(trans != NULL);
    assert(block != NULL);
    assert(cmd != NULL);

    DBG fprintf(fp, "mov rax, r11\n");
    WriteCmd(MOV_RAX_R11);

    DBG fprintf(fp, "add rax, %d\n", cmd->dest.var->offset);
    WriteAddRegNum(RAX, cmd->dest.var->offset);

    DBG fprintf(fp, "push rax\n");
    WritePushReg(RAX);

    return NOERR;
}

int EnlargeR11(FuncIR* function, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);

    DBG fprintf(fp, "add r11, %d\n", function->table.capacity * 8);

    WriteAddRegNum(R11, function->table.capacity * 8);

    return NOERR;
}

int ReduceR11(FuncIR* function, BinTrans* trans)
{
    assert(function != NULL);
    assert(trans != NULL);

    DBG fprintf(fp, "sub r11, %d\n", function->table.capacity * 8);

    WriteCmd(SUB_R11_NUM);
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
    assert(ir != NULL);

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
    assert(oper != NULL);
    assert(trans != NULL);

    *(unsigned int*)(trans->exebuff + trans->len) = oper->opcode;
    trans->len += oper->size;

    return NOERR;
}

int WriteAbsPtr(BinTrans* trans, uint64_t ptr)
{
    assert(trans != NULL);

    *(uint64_t*) (trans->exebuff + trans->len) = ptr;
    trans->len += sizeof(uint64_t);

    return NOERR;
}

int WriteNum(BinTrans* trans, int num)
{
    assert(trans != NULL);

    *(int*) (trans->exebuff + trans->len) = num;
    trans->len += sizeof(int);

    return NOERR;
}

int WriteCall(BinTrans* trans, char* func)
{
    assert(trans != NULL);
    assert(func != NULL);

    Operation call = {CALL, SIZE_CALL};
    WriteBinCmd(trans, &call);

    int ip = FindLabel(trans, func, INITIAL);

    WriteNum(trans, ip - sizeof(int) - trans->len);

    return NOERR;
}

int WriteUnCndJump(BinTrans* trans, char* func, char* block)
{
    assert(trans != NULL);
    assert(func != NULL);
    assert(block != NULL);

    Operation jmp = {JMP, SIZE_JMP};
    WriteBinCmd(trans, &jmp);

    int ip = FindLabel(trans, func, block);

    WriteNum(trans, ip - sizeof(int) - trans->len);

    return NOERR;
}

int WriteCndJump(BinTrans* trans, char* func, char* block)
{
    assert(trans != NULL);
    assert(func != NULL);
    assert(block != NULL);

    Operation jne = {JNE, SIZE_JNE};
    WriteBinCmd(trans, &jne);

    int ip = FindLabel(trans, func, block);

    WriteNum(trans, ip - sizeof(int) - trans->len);

    return NOERR;
}

int FindLabel(BinTrans* trans, char* func, char* block)
{
    assert(trans != NULL);
    assert(func != NULL);
    assert(block != NULL);

    int ip = 0;
    for (int i = 0; i < trans->labelstable.size; i++)
    {
        if (strcmp(trans->labelstable.labels[i].func, func) == 0)
        {
            if (strcmp(trans->labelstable.labels[i].block, block) == 0)
            {
                ip = trans->labelstable.labels[i].ip;
                break;
            }
        }
    }

    return ip;
}

int AddLabel(BinTrans* trans, char* func, char* block)
{
    assert(trans != NULL);
    assert(func != NULL);
    assert(block != NULL);

    trans->labelstable.labels[trans->labelstable.size].block = block;
    trans->labelstable.labels[trans->labelstable.size].func = func;
    trans->labelstable.labels[trans->labelstable.size].ip = trans->len;
    trans->labelstable.size += 1;

    return NOERR;
}

int Print(int num)
{
    printf("%d ", num);

    return NOERR;
}

int DumpBuffer(BinTrans* trans)
{
    assert(trans != NULL);

    for (int i = 0; i < trans->len; i++)
    {
        fprintf(dump, "%02x", trans->exebuff[i]);
    }
    fprintf(dump, "\n");

    return NOERR;
}

int Scanf(int* num)
{
    scanf("%d", num);

    return NOERR;
}
