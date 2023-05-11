#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "opcodes.h"

struct Operation
{

}

struct Label
{
    char* name;
    int offset;
};

struct LabelsTable
{
    Label* labels;
    int capacity;
    int size;
};

struct BinTrans
{
    char* membuff;
    char* exebuff;
    int len;
    LabelsTable labelstable;
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

#endif // TRANSLATOR_H
