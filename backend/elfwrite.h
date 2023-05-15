#ifndef ELFWRITE_H
#define ELFWRITE_H

#include "translator.h"

int CreateElfFile(BinTrans* trans);

int CreateElfHeader(FILE* elf);

#endif // ELFWRITE.H
