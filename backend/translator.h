#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include <cstdint>

#include "ir.h"
#include "opcodes.h"

struct Operation
{
    enum OPCODES opcode;
    enum OPCODES_SIZES size;
};

struct Label
{
    char* func;
    char* block;
    int ip;
};

struct LabelsTable
{
    Label* labels;
    int capacity;
    int size;
};

struct BinTrans
{
    unsigned char* membuff;
    unsigned char* exebuff;
    int len;
    LabelsTable labelstable;

    char* formatout;
    char* formatin;
};

int TranslateIR(IR* ir, BinTrans* trans);

int TranslateFunc(FuncIR* function, BinTrans* trans);

int TranslateBlock(FuncIR* function, BlockIR* block, BinTrans* trans);

int TranslateCommand(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateVar(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateEq(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateArithOper(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateRet(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateCall(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateIf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslatePrintf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateScanf(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateParamOut(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateParamIn(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int TranslateParamS(FuncIR* function, BlockIR* block, IRCommand* cmd, BinTrans* trans);

int ExternFunctions();

int EnlargeR11(FuncIR* function, BinTrans* trans);

int ReduceR11(FuncIR* function, BinTrans* trans);

int CountLabels(IR* ir);

int WriteBinCmd(BinTrans* trans, Operation* oper);

int WriteAbsPtr(BinTrans* trans, uint64_t ptr);

int WriteNum(BinTrans* trans, int num);

int WriteUnCndJump(BinTrans* trans, char* func, char* block);

int WriteCndJump(BinTrans* trans, char* func, char* block);

int AddLabel(BinTrans* trans, char* func, char* block);

int BinTransCtor(IR* ir, BinTrans* trans);

int BinTransDtor(BinTrans* trans);

int WriteCall(BinTrans* trans, char* func);

int FindLabel(BinTrans* trans, char* func, char* block);

int DumpBuffer(BinTrans* trans);

int Print(int num);

int Scanf(int* num);

#endif // TRANSLATOR_H
