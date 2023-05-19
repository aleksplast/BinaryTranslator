#ifndef ELFWRITE_H
#define ELFWRITE_H

#include "translator.h"

int CreateElfFile(BinTrans* trans, const char* filename);

int CreateElfHeader(FILE* elf);

int CreateProgrammHeader(FILE* elf, int stlib);

int WriteBinFunc(FILE* func, FILE* elf);

#endif // ELFWRITE.H
