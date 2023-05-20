#include <stdio.h>
#include <locale.h>
#include <cstdlib>
#include <time.h>
#include <time.h>
#include <sys/mman.h>
#include <cstring>

#include "frontend/frontend.h"
#include "backend/ir.h"
#include "backend/translator.h"
#include "backend/backend.h"
#include "middleend/middleend.h"
#include "backend/elfwrite.h"

int inline RunCode(unsigned char* buff);

int main(int argc, char* argv[])
{
    const char* input = GetComArg(argc, argv);
    const char data[50] = "lib/data.txt";
    const char elffile[50] = "elffile";
    FILE* fp = fopen("dumpfile.txt", "w");
    fclose(fp);

    setlocale(LC_ALL, "Rus");
    Source datasrc = InputHandler(input);

    FILE* asmprog = fopen("asmprog.txt", "w");

    Tree tree = {};
    char log[STRSIZE] = "./lib/graphlog.htm";

    char** namestable = (char**) calloc(datasrc.size, sizeof(char*));
    char** oldtable = namestable;
    char** dtable = namestable;

    char* datastr = TranslitIntoEnglish(datasrc.ptr, datasrc.size);
    int len = strlen(datastr);

    Node** nodes = LexicAnalizer(datastr, &namestable);

    Node* node = GetGrammar(nodes, strlen(datastr) + 2);

    TreeCtor(&tree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    free(tree.anchor);
    tree.anchor = node;

    CreateAncestor(node, NULL, &tree);
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);
//    SimplifyCode(&tree);

    DataPrint(node);
    NodeDetor(node);
    node = ReadFromStandart(data);

    tree.anchor = node;
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);

    CreateAncestor(tree.anchor, NULL, &tree);

    CreateAsmProgramm(tree.anchor, asmprog);

    IR ir = {};
    TreeToIR(&tree, &ir);

    BinTrans trans = {};
    BinTransCtor(&ir, &trans);
    TranslateIR(&ir, &trans);

    TranslateIR(&ir, &trans);

    CreateElfFile(&trans, elffile);

//     mprotect(trans.exebuff, trans.len + 1, PROT_EXEC);
//
//     RunCode(trans.exebuff);
//
//     mprotect(trans.exebuff, trans.len + 1, PROT_WRITE);
    BinTransDtor(&trans);
    IrDtor(&ir);
    NodeDtor(tree.anchor);

    while (*dtable != NULL)
    {
        free(*dtable);
        dtable += 1;
    }

    free(datasrc.Strings);
    free(datasrc.ptr);
    free(nodes);
    free(namestable);
    free(datastr);


    return 0;
}

int inline RunCode(unsigned char* buff)
{
    void (*func) (void) = (void (*)(void))(buff);

    func();

    return 0;
}
