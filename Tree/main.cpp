#include "tree.h"

int main(void)
{
    FILE *htmfile = fopen("LogStuff/TreeLogs.htm", "w");
    FILE *txtfile = fopen("LogStuff/TreeExport.txt", "r");

    Tree tree = {};
    Node root = {};
    
    TreeInit(&tree, &root, "\0", htmfile, __LINE__);

    ImportTree(&tree, txtfile, __LINE__);

    KillTheEntireFamily(&tree, __LINE__);

    fclose(htmfile);
    system("xdg-open LogStuff/TreeLogs.htm");


    return 0;
}