#ifndef ELFWRITE_H
#define ELFWRITE_H

#include "translator.h"

int CreateElfFile(BinTrans* trans, const char* filename);

int CreateElfHeader(FILE* elf);

int CreateProgrammHeader(FILE* elf);

int AddBinFunc(FILE* func, FILE* elf);

#endif // ELFWRITE.H
