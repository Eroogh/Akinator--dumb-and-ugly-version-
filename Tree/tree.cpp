#include "tree.h"


const ERRTREESTRUCT ErrorMessages[] =
{
    {POISONERR,                            "THERE'S POISON IN NODE"},
    {SAMEKIDS,                             "LEFT KID'S POINTER EQUALS TO THE RIGHT KID'S ONE"},
    {LEFTADOPTED,                          "LEFT KID IS ADOPTED (NO ONE POINTING TO THE LEFT KID)"},
    {RIGHTADOPTED,                         "RIGHT KID IS ADOPTED (NO ONE POINTING TO THE RIGHT KID)"},
    {CYCLEORSMTH,                          "THERE'S A CYCLE IN TREE OR OTHER PROBLEM"},
    {TRYING_TO_GIVE_BIRTH_TO_EXISTING_KID, "ABORT!!! YOU ARE TRYING TO GIVE A BIRTH TO ALREADY EXISTING KID"},
    {POISONED_PARENT,                      "ABORT!!! PARENT IS FUCKED UP CALL CUSTODY ASAP!!!"}
};

ERRTREE TreeInit(Tree *tree, Node *root, const char * RootText, FILE* htmfile, long unsigned line)
{
    assert(tree);
    assert(root);
    assert(RootText);
    assert(htmfile);

    (tree->root) = root;
    NodeInit(NULL, root, RootText);
    tree->size = 1;
    tree->htmfile = htmfile;
    // todo add ability to turn off dumps
    LogHeader(htmfile);

    return TreeLogWrite(tree, NormLogWidth, __func__, line);
}

void KillTheEntireFamily(Tree *tree, long unsigned line)
{
    assert(tree);
    assert(tree->root);

    TreeDestroy(tree, &(tree->root), line);

    LogEnd(tree->htmfile, line);
}

ERRTREE TreeDestroy(Tree *tree, Node **root, long unsigned line) // update tree size with new function that takes only node
{
    assert(root);
    assert(*root);
    assert(tree);

    //fprintf(stderr, "%s - treedestroy 1\n", (*root)->data.text);

    if ((*root)->LeftKid != NULL)
        TreeDestroy(tree, &((*root)->LeftKid), line); 

    //fprintf(stderr, "%s - treedestroy 2\n", (*root)->data.text);

    if ((*root)->RightKid != NULL)
        TreeDestroy(tree, &((*root)->RightKid), line);

    //fprintf(stderr, "%s - treedestroy 3\n", (*root)->data.text);

    fprintf(tree->htmfile, "\t<p>\nDestroying Node - %s</p>\n", (*root)->data.text);
    fprintf(stderr, "Destroying Node - %s\n", (*root)->data.text);
    NodeDestroy(root);

    fprintf(stderr, "%p - treedestroy\n", *root);

    return TreeLogWrite(tree, NormLogWidth, __func__, line);
}

void NodeDestroy(Node **node)
{
    assert(node);
    assert(*node);
    // fprintf(stderr, "%s, %p - nodedestroy\n", (*node)->data.text, *node);

    free(((*node)->data).text);

    if ((*node)->Parent != NULL) // no need in nulling kid's pointer
    {
        if (((*node)->Parent)->LeftKid != NULL)
        {
            fprintf(stderr, "first if\n");
            if (*node == ((*node)->Parent)->LeftKid)
                ((*node)->Parent)->LeftKid = NULL;
        }
        else
        {
        if (((*node)->Parent)->RightKid != NULL)
        {
            fprintf(stderr, "second if\n");
            if(*node == ((*node)->Parent)->RightKid)
                ((*node)->Parent)->RightKid = NULL;
        }}
    }
    *node = NULL;
}

// NODE_DESTROY(node) { NodeDestroy(&node);
    // (node) = NULL;
// }
void NodeInit(Node* parent, Node *node, const char * text)
{
    assert(node);
    assert(text);

    node->Parent = parent;
    unsigned int len = strlen(text);

    if (len >= PoisonStringLen)
        node->data.text = (char*)calloc(len + 1, sizeof(char));
    else
        node->data.text = (char*)calloc(PoisonStringLen, sizeof(char));

    strcpy(node->data.text, text);
    node->LeftKid = NULL;
    node->RightKid = NULL;
}

ERRTREE GiveBirth(Tree *tree, Node *parent, Node* child, LeftOrRight LOR, const char * kidtext)
{
    assert(tree);
    assert(child);
    assert(kidtext);

    NodeInit(parent, child, kidtext);

    if (parent == NULL)
    {
        return TreeLogWrite(tree, NormLogWidth, __func__, __LINE__);
    }

    if (strcmp((parent->data).text, PoisonString) == 0)
    { 
        PrintError(POISONED_PARENT, tree->htmfile, __LINE__);
        NodeDestroy(&child);
        return POISONED_PARENT;
    }

    if (LOR == left)
    {
        if (parent->LeftKid != NULL)
        {
            PrintError(TRYING_TO_GIVE_BIRTH_TO_EXISTING_KID, tree->htmfile, __LINE__);
            NodeDestroy(&child);
            return TRYING_TO_GIVE_BIRTH_TO_EXISTING_KID;
        }
        parent->LeftKid = child;
    }
    if (LOR == right)
    {
        if (parent->RightKid != NULL)
        {
            PrintError(TRYING_TO_GIVE_BIRTH_TO_EXISTING_KID, tree->htmfile, __LINE__);
            NodeDestroy(&child);
            return TRYING_TO_GIVE_BIRTH_TO_EXISTING_KID;
        }
        parent->RightKid = child;
    }
    tree->size++;

    return TreeLogWrite(tree, NormLogWidth, __func__, __LINE__);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PrintError(ERRTREE err, FILE * htmfile, long unsigned line)
{
    assert(htmfile);

    if (err == NOERROR)
        return;

    size_t NumberOfErrors = sizeof(ErrorMessages) / sizeof(ErrorMessages[0]);

    for (size_t i = 0; i < NumberOfErrors; i++)
    {
        if (err == ErrorMessages[i].errenum)
            fprintf(htmfile, "\t<p style=\"color:Red;\">\n"
                    "%s, line = %lu\n\n", 
                    ErrorMessages[i].message, line);
    }
}

void NodeVerify(Node* node, ERRTREE *err, FILE * htmfile, long unsigned line)
{
    assert(err);
    assert(htmfile);
    if (node == NULL)
        return;

    //fprintf(stderr, "%lg\n", (node->data).val);

    CheckText((node->data).text, err);
    CheckKids(node->LeftKid, node->RightKid, err);
    CheckLeftAdopted (node->LeftKid,  node,  err);
    CheckRightAdopted(node->RightKid, node,  err);

    PrintError(*err, htmfile, line);
}

void TreeVerify(Node* root, size_t size, ERRTREE *err, FILE *htmfile,  long unsigned line, bool UpdateNodesCounter)
{
    assert(err);
    assert(htmfile);
    if (root == NULL)
        return;

    NodeVerify(root, err, htmfile, line);

    static size_t nodescounter = 0;

    if (UpdateNodesCounter)
    {
        nodescounter = 0;
    }

    nodescounter++;

    CheckNodesCounter(nodescounter, size, err);

    //fprintf(stderr, "%zu\n", nodescounter);

    TreeVerify(root->LeftKid,  size, err, htmfile, line, false);
    TreeVerify(root->RightKid, size, err, htmfile, line, false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CheckText(const char * text, ERRTREE *err)
{
    assert(text);
    assert(err);
    if (*err != NOERROR)
        return;

    if (strcmp(text, PoisonString) == 0)
        *err = POISONERR;
}

void CheckKids(Node *Left, Node *Right, ERRTREE *err)
{
    assert(err);
    if (*err != NOERROR)
        return;

    if (Left == NULL && Right == NULL)
        return;
    if (Left == Right)
        *err = SAMEKIDS;
}

void CheckLeftAdopted(Node* kid, Node* node, ERRTREE *err)
{
    assert(err);
    if (*err != NOERROR)
        return;
    
    if (node->LeftKid != kid)
        *err = LEFTADOPTED;
}

void CheckRightAdopted(Node* kid, Node* node, ERRTREE *err)
{
    assert(err);
    if (*err != NOERROR)
        return;
    
    if (node->RightKid != kid)
        *err = RIGHTADOPTED;
}

void CheckNodesCounter(size_t counter, size_t size, ERRTREE *err)
{
    assert(err);
    if (*err != NOERROR)
        return;
    
    if(counter > size)
        *err = CYCLEORSMTH;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const size_t MaxDumpFileNameSize = 100;

ERRTREE TreeLogWrite(Tree *tree, int LogWidth, const char * funcname, unsigned long line)
{
    assert(funcname);
    assert(tree);

    static int NumOfDumps = 1;

    char DumpFileName[MaxDumpFileNameSize] = {};
    sprintf(DumpFileName, "Dump_%d", NumOfDumps);

    TreeSvgDump(tree, DumpFileName, funcname);

    ERRTREE err = TreeLog(tree, LogWidth, DumpFileName, line);
    NumOfDumps++;

    return err;
}

void LogHeader(FILE *htmfile)
{
    assert(htmfile);
    fprintf(htmfile,"<!DOCTYPE html>\n"
                    "<html lang=\"en\">\n"
                    "<head>\n"
                    "\t<title>My Log Yopta</title>\n"
                    "</head>\n\n"
                    "<body>\n"
            );
}

void LogEnd(FILE* htmfile, long unsigned line)
{
    assert(htmfile);
    fprintf(htmfile,"\t<p>\nline = %lu\nTree Destroyed\n</p>\n"
                    "</body>\n"
                    "</html>\n",
            line);
}

ERRTREE TreeLog(Tree *tree, int LogWidth, const char* DumpFileName, long unsigned line)
{
    assert(tree);
    assert(DumpFileName);

    fprintf(tree->htmfile,"\t<h2>DUMP</h2>\n"
                    "\t<pre>\n"     
            );

    ERRTREE err = NOERROR;

    TreeVerify(tree->root, tree->size, &err, tree->htmfile, line, true);

    if(err != NOERROR)
    {
    PrintError(err, tree->htmfile, line);
    fprintf(tree->htmfile, "here's your tree:\n</p>\n");
    }
    else
    {
    fprintf(tree->htmfile, "\t<p>\nline = %ld\nhere's your tree:\n</p>\n", line);
    }

    char *SvgDumpFileName = (char *)calloc(strlen(DumpFileName) + 5, sizeof(char));

    sprintf(SvgDumpFileName, "%s.svg", DumpFileName);

    fprintf(tree->htmfile, "\t<img src = \"%s\" width = \"%d\"></img>\n", SvgDumpFileName, LogWidth);

    free(SvgDumpFileName);

    return err;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NodeDotDump(Node* node, FILE *dotfile)
{
    assert(node);
    assert(dotfile);

    DrawNode(dotfile, node);
    DrawEdge(dotfile, node, node->LeftKid);
    DrawEdge(dotfile, node, node->RightKid);
}

void TreeDotDump(Node *root, FILE *dotfile)
{
    assert(dotfile);
    if (root == NULL)
        return;

    fprintf(stderr, "%p - dot\n", root);

    NodeDotDump(root, dotfile);

    TreeDotDump(root->LeftKid, dotfile);
    TreeDotDump(root->RightKid, dotfile);
}

void TreeSvgDump(Tree *tree, const char *filename, const char * funcname) 
{
    assert(tree);
    assert(filename);
    assert(funcname);

    char *dotfilename = (char *)calloc(strlen(filename) + 14, sizeof(char)); //
    sprintf(dotfilename, "LogStuff/%s.dot", filename);
    fprintf(stderr, "%s, %s\n", dotfilename, funcname);

    FILE *dotfile = fopen(dotfilename, "w");

    DrawHeader(dotfile);
    TreeDotDump(tree->root, dotfile);
    fprintf(dotfile, "}");
    fclose(dotfile); 

    // const char * command = "dot -Tsvg LogStuff/.dot > LogStuff/.svg\n"

    char *makesvg = (char *)calloc(2 * strlen(filename) + 114, sizeof(char)); // strlen(command)
    sprintf(makesvg, "dot -Tsvg LogStuff/%s.dot > LogStuff/%s.svg\n", filename, filename);
    system(makesvg);

    free(makesvg);
    free(dotfilename);
}

void DrawHeader(FILE *dotfile)
{
    assert(dotfile);

    fprintf(dotfile,
            "digraph Tree {\n"
            "\trankdir = TB;\n\n"
            "\tnode [shape=egg, fontname=\"Impact\", fontsize=14];\n\n");
}

#define NodeColor "#c172e6ff"

void DrawNode(FILE *dotfile, Node *node)
{
    assert(dotfile);
    assert(node);

    fprintf(stderr, "%s, %p - drawnode\n", (node->data).text, node);

    if (node->LeftKid == NULL && node->RightKid == NULL)
    {
        fprintf(stderr, "first if\n");
        fprintf(dotfile,
                "\telem%p [label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
                "\t\t<TR><TD BGCOLOR=\"" NodeColor "\" COLSPAN=\"2\">%s</TD></TR>\n"
                "\t\t<TR><TD BGCOLOR=\"" NodeColor "\">NULL</TD>\n"
                "\t\t\t<TD BGCOLOR=\"" NodeColor "\">NULL</TD></TR>\n"
                "\t\t</TABLE>>];\n\n",
                node, (node->data).text);
        return;
    }
    if (node->LeftKid == NULL && node->RightKid != NULL)
    {
        fprintf(stderr, "second if, rightkid - %p\n", node->RightKid);
        fprintf(dotfile,
                "\telem%p [label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
                "\t\t<TR><TD BGCOLOR=\"" NodeColor "\" COLSPAN=\"2\">%s</TD></TR>\n"
                "\t\t<TR><TD BGCOLOR=\"" NodeColor "\">NULL</TD>\n"
                "\t\t\t<TD BGCOLOR=\"" NodeColor "\">%s</TD></TR>\n"
                "\t\t</TABLE>>];\n\n",
                node, (node->data).text, ((node->RightKid)->data).text);
        return;
    }
    if (node->RightKid == NULL && node->LeftKid != NULL)
    {
        fprintf(stderr, "third if\n");
        fprintf(dotfile,
                "\telem%p [label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
                "\t\t<TR><TD BGCOLOR=\"" NodeColor "\" COLSPAN=\"2\">%s</TD></TR>\n"
                "\t\t<TR><TD BGCOLOR=\"" NodeColor "\">%s</TD>\n"
                "\t\t\t<TD BGCOLOR=\"" NodeColor "\">NULL</TD></TR>\n"
                "\t\t</TABLE>>];\n\n",
                node, (node->data).text, ((node->LeftKid)->data).text);
        return;
    }

    fprintf(dotfile,
            "\telem%p [label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
            "\t\t<TR><TD BGCOLOR=\"" NodeColor "\" COLSPAN=\"2\">%s</TD></TR>\n"
            "\t\t<TR><TD BGCOLOR=\"" NodeColor "\">%s</TD>\n"
            "\t\t\t<TD BGCOLOR=\"" NodeColor "\">%s</TD></TR>\n"
            "\t\t</TABLE>>];\n\n",
            node, (node->data).text, ((node->LeftKid)->data).text, ((node->RightKid)->data).text);
}

#undef NodeColor

void DrawEdge(FILE *dotfile, Node *node1, Node *node2)
{
    assert(dotfile);
    if (node2 == NULL)
        return;
    if (node1 == NULL)
        return;

    fprintf(dotfile, "\telem%p -> elem%p\n\n",
                    node1, node2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ExportTree(Node* root, FILE * stream)
{
    assert(root);
    assert(stream);

    fprintf(stream, "(\"%s\" ", root->data.text);

    if (root->LeftKid != NULL)
        ExportTree(root->LeftKid, stream);
    else
        fprintf(stream, "%s ", "nil");
    
    if (root->RightKid != NULL)
        ExportTree(root->RightKid, stream);
    else
        fprintf(stream, "%s ", "nil");

    fprintf(stream, ")");
}

const size_t maxfilesize = 10000;

const size_t maxnamesize = 100;

void ImportTree(Tree *tree, FILE *txtfile, unsigned int line)
{
    assert(tree);
    assert(txtfile);

    char *buffer = (char *)calloc(maxfilesize + 1, sizeof(char));
    size_t size = ReadToBuffer(buffer, txtfile);

    size_t pos = 0;
    ReadNode(tree, tree->root, left, NULL, buffer, &pos, line);
    free(buffer);
}

void ReadNode(Tree* tree, Node* node, LeftOrRight LOR, Node* parent, const char * buffer, size_t *pos, unsigned int line)
{
    assert(tree); 
    assert(buffer);
    assert(pos);

    if (buffer[*pos] == '(')
    {
        fprintf(stderr, "%c\n", buffer[*pos]);
        *pos += sizeof(char); // skips '('
        char * text = ReadNodeName(buffer, pos);
        GiveBirth(tree, parent, node, LOR, text);
        free(text);
        Node LeftKid  = {};
        NodeInit(node, &LeftKid, "\0");
        ReadNode(tree, &LeftKid, left, node, buffer, pos, line);
        Node RightKid = {};
        NodeInit(node, &RightKid, "\0");
        ReadNode(tree, &RightKid, right, node, buffer, pos, line);
        *pos += sizeof(char); // skips ')'
        return;
    }
    *pos += 3 * sizeof(char); // skips "nil"
}

char * ReadNodeName(const char * buffer, size_t *pos)
{
    assert(buffer);
    assert(pos);

    char *res = (char *)calloc(maxnamesize + 1, sizeof(char));
    fprintf(stderr, "%c\n", buffer[*pos]);
    *pos += sizeof(char); // skips "
    for (size_t i = 0; buffer[*pos] != '\"' && i <= maxnamesize; i++)
    {
        fprintf(stderr, "%c\n", buffer[*pos]);
        res[i] = buffer[*pos];
        *pos += sizeof(char);
    }
    fprintf(stderr, "%c\n", buffer[*pos]);
    *pos += sizeof(char); // skips "
    return res;
}

size_t ReadToBuffer(char * buffer, FILE * txtfile)
{
    assert(buffer);
    assert(txtfile);

    int c = 0;
    size_t i = 0;
    while ((c = getc(txtfile)) != EOF  && i <= maxfilesize)
    {
        if (isspace(c))
            continue;
        buffer[i] = c;
        i++;
    }

    fclose(txtfile);
    return i; // size of readen input
}