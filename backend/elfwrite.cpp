#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <elf.h>

#include "elfwrite.h"

int CreateElfFile(BinTrans* trans)
{
    FILE* elf = fopen("outtrans", "w");

    CreateElfHeader(elf);


}

int CreateElfHeader(FILE* elf)
{

}
