#include <stdio.h>
#include <locale.h>
#include <cstdlib>
#include <time.h>

#include "frontend/frontend.h"
#include "backend/ir.h"
#include "backend/translator.h"
#include "backend/backend.h"
#include "middleend/middleend.h"
#include "sys/mman.h"

int inline RunCode(unsigned char* buff);

int main(int argc, char* argv[])
{
    const char* input = GetComArg(argc, argv);
    const char data[50] = "lib/data.txt";
    FILE* fp = fopen("dumpfile.txt", "w");
    fclose(fp);

    setlocale(LC_ALL, "Rus");
    Source datasrc = InputHandler(input);

    FILE* asmprog = fopen("asmprog.txt", "w");

    Tree tree = {};
    char log[STRSIZE] = "./lib/graphlog.htm";

    char** namestable = (char**) calloc(datasrc.size, sizeof(char*));

    char* datastr = TranslitIntoEnglish(datasrc.ptr, datasrc.size);

    Node** nodes = LexicAnalizer(datastr, &namestable);

    Node* node = GetGrammar(nodes);

    TreeCtor(&tree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    tree.anchor = node;

    CreateAncestor(node, NULL, &tree);
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);
    SimplifyCode(&tree);

    DataPrint(node);
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

    mprotect(trans.exebuff, trans.len + 1, PROT_EXEC);

    RunCode(trans.exebuff);

    mprotect(trans.exebuff, trans.len + 1, PROT_WRITE);
    BinTransDtor(&trans);
    IrDtor(&ir);


    return 0;
}

int inline RunCode(unsigned char* buff)
{
    void (*func) (void) = (void (*)(void))(buff);

    func();

    return 0;
}
