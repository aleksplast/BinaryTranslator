#ifndef TRANSLATOR_H
#define TRANSLATOR_H

int TranslateIR(IR* ir);

int TranslateFunc(FuncIR* function);

int TranslateBlock(FuncIR* function, BlockIR* block);

int TranslateCommand(FuncIR* function, BlockIR* block, IRCommand* cmd);

int TranslateVar(FuncIR* function, BlockIR* block, IRCommand* cmd);

int TranslateEq(FuncIR* function, BlockIR* block, IRCommand* cmd);

int TranslateArithOper(FuncIR* function, BlockIR* block, IRCommand* cmd);

int TranslateRet(FuncIR* function, BlockIR* block, IRCommand* cmd);

int TranslateCall(FuncIR* function, BlockIR* block, IRCommand* cmd);

int TranslateIf(FuncIR* function, BlockIR* block, IRCommand* cmd);

#endif // TRANSLATOR_H
