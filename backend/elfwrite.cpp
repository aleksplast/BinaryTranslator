#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstddef>
#include <elf.h>

#include "elfwrite.h"

size_t SIZEBUFFER =  4096;
size_t SIZESTR = 100;

unsigned char BinPrint[] = {0x50, 0x53, 0x51, 0x52, 0x56, 0x48, 0x31, 0xc9, 0x48, 0x31, 0xd2, 0x4d, 0x31, 0xed, 0x83, 0xf8, 0x00, 0x7d, 0x02, 0xf7, 0xd8, 0x66, 0x31, 0xd2, 0x66, 0xf7, 0xf1, 0x80, 0xc2, 0x30, 0xeb, 0xf5, 0x66, 0x83, 0xf8, 0x00, 0x74, 0x0c, 0xf6, 0xf1, 0x50, 0x86, 0xc4, 0x04, 0x30, 0x58, 0x30, 0xe4, 0xeb, 0xee, 0xe8, 0x06, 0x00, 0x00, 0x00, 0x5e, 0x5a, 0x59, 0x5b, 0x58, 0xc3, 0x50, 0x57, 0x56, 0x52, 0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00, 0x4c, 0x89, 0xe6, 0x4c, 0x89, 0xea, 0x4c, 0x29, 0xe2, 0x0f, 0x05, 0x4d, 0x89, 0xe5, 0x5a, 0x5e, 0x5f, 0x58, 0xc3};


int CreateElfFile(BinTrans* trans, const char* filename)
{
    FILE* elf = fopen(filename, "wb");

    char chm[SIZESTR] = "";
    sprintf(chm, "chmod +x %s", filename);

    CreateElfHeader(elf);

    CreateProgrammHeader(elf);

    unsigned char new8[SIZESTR] = {0, 0, 0, 0, 0 , 0, 0, 0};
    fwrite(new8, sizeof(char), 8, elf);

    unsigned char test[SIZESTR] = {PUSH_RAX, PUSH_RBX, POP_RAX, 0x6a, 0x3c, 0x58, 0x31, 0xff, 0x0f, 0x05, 0x00};

//    fwrite(test, sizeof(char), 11, elf);
    fwrite(trans->exebuff, sizeof(char), SIZEBUFFER, elf);
    fwrite(trans->exebuff, sizeof(char), SIZEBUFFER, elf);

    fclose(elf);

    system(chm);

    return NOERR;
}

int CreateElfHeader(FILE* elf)
{
    Elf64_Ehdr head = {};
    head.e_ident[EI_MAG0] = 0x7f;
    head.e_ident[EI_MAG1] = 'E';
    head.e_ident[EI_MAG2] = 'L';
    head.e_ident[EI_MAG3] = 'F';
    head.e_ident[EI_CLASS] = ELFCLASS64;
    head.e_ident[EI_DATA] = ELFDATA2LSB;
    head.e_ident[EI_VERSION] = 0x01;
    head.e_ident[EI_OSABI] = 0x00;
    head.e_type = ET_EXEC;
    head.e_version = 1;
    head.e_machine = 0x3e;
    head.e_entry = 0x400080;
    head.e_shoff = 0x00;
    head.e_phoff = 0x40;
    head.e_flags = 0x00;
    head.e_ehsize = 0x40;
    head.e_phentsize = 0x38;
    head.e_phnum = 0x01;

    fwrite(&head, sizeof(head), 1, elf);

    return NOERR;
}

int CreateProgrammHeader(FILE* elf)
{
    Elf64_Phdr head = {};

    head.p_type = 0x01;
    head.p_filesz = 2 * SIZEBUFFER + 0x80;
    head.p_memsz = 2 * SIZEBUFFER + 0x80;
    head.p_offset = 0x00;
    head.p_paddr = 0x400000;
    head.p_vaddr = 0x400000;
    head.p_align = 0x2000;
    head.p_flags = 0x07;

    fwrite(&head, sizeof(head), 1, elf);

    return NOERR;
}
