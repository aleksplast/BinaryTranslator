#include <stdio.h>
#include <locale.h>
#include <cstdlib>

#include "frontend/frontend.h"
#include "translator/translator.h"
#include "middleend/middleend.h"

int main(int argc, char* argv[])
{
    const char* input = GetComArg(argc, argv);
    const char data[50] = "lib/data.txt";

    setlocale(LC_ALL, "Rus");
    Source datasrc = InputHandler(input);

    Tree tree = {};
    char log[STRSIZE] = "./lib/graphlog.htm";

    char** namestable = (char**) calloc(datasrc.size, sizeof(char*));

    char* datastr = TranslitIntoEnglish(datasrc.ptr, datasrc.size);

    Node** nodes = LexicAnalizer(datastr, &namestable);

    Node* node = GetGrammar(nodes);
    printf("HERE\n");

    TreeCtor(&tree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    tree.anchor = node;

    CreateAncestor(node, NULL, &tree);
    ChangeCoreFunctions(node);
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);
    SimplifyCode(&tree);

    printf("HERE\n");
    DataPrint(node);
    node = ReadFromStandart(data);

    tree.anchor = node;
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);

    CreateAncestor(tree.anchor, NULL, &tree);

    return 0;
}
