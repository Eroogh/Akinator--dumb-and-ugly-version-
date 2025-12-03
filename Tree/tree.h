#ifndef TREE_H    
#define TREE_H 
#include <stdio.h> // 🤓🤓🤓🤓🤓🤓🤓🤓🤓 - мой ментор
#include <malloc.h>
#include <math.h>
#include <stdlib.h> 
#include <assert.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    NOERROR = 0,
    POISONERR,
    SAMEKIDS,
    LEFTADOPTED,
    RIGHTADOPTED,
    CYCLEORSMTH,
    TRYING_TO_GIVE_BIRTH_TO_EXISTING_KID,
    WHO_ARE_YOU_GIVING_BIRTH_TO,
    POISONED_PARENT
} ERRTREE;

typedef struct 
{
    ERRTREE errenum;
    const char *message;
} ERRTREESTRUCT;

typedef struct
{
    char *text;
} Node_data;

struct Node
{
    Node_data data;
    Node *Parent;
    Node *LeftKid;
    Node *RightKid;
};

typedef struct 
{
    FILE *htmfile;
    Node *root;
    size_t size;
} Tree;

typedef enum
{
    left,
    right

} LeftOrRight;

const int NormLogWidth = 400;

const int PoisonVal = 666666;

const unsigned int PoisonStringLen = strlen("ITS POISON WAAAAAAAAAAAAAAAAAAAA") + 1;

const char PoisonString[PoisonStringLen] = "ITS POISON WAAAAAAAAAAAAAAAAAAAA";

ERRTREE TreeInit         (Tree *tree, Node *root, const char * RootText, FILE* htmfile, long unsigned line);
ERRTREE GiveBirth        (Tree *tree, Node *parent, Node* child, LeftOrRight LOR, const char * kidtext);
void NodeInit            (Node* parent, Node *node, const char * text);

void NodeDestroy         (Node **node);
ERRTREE TreeDestroy      (Tree *tree, Node **root, long unsigned line);
void KillTheEntireFamily (Tree *tree, long unsigned line);

void TreeReallocIfNeeded (Tree *tree);

void NodeVerify          (Node *node, ERRTREE *err, FILE *htmfile, long unsigned line);
void TreeVerify          (Node* root, size_t size, ERRTREE *err, FILE *htmfile,  long unsigned line, bool UpdateNodesCounter);
void CheckText           (const char* text, ERRTREE *err);
void CheckKids           (Node *Left, Node *Right, ERRTREE *err);
void CheckLeftAdopted    (Node *kid, Node *node, ERRTREE *err);
void CheckRightAdopted   (Node *kid, Node *node, ERRTREE *err);
void CheckNodesCounter   (size_t counter, size_t size, ERRTREE *err);

void PrintError          (ERRTREE err, FILE * htmfile, long unsigned line);

void NodeDotDump         (Node *node, FILE *dotfile);
void TreeDotDump         (Node *root, FILE *dotfile);
void TreeSvgDump         (Tree *tree, const char *filename, const char * funcname);
void DrawHeader          (FILE *dotfile);
void DrawNode            (FILE *dotfile, Node *node);
void DrawEdge            (FILE *dotfile, Node *node1, Node *node2);

ERRTREE TreeLogWrite     (Tree *tree, int LogWidth, const char * funcname, unsigned long line);
ERRTREE TreeLog          (Tree *tree, int LogWidth, const char *DumpFileName, long unsigned line);
void LogHeader           (FILE *htmfile);
void LogEnd              (FILE *htmfile, long unsigned line);

void ExportTree          (Node *root, FILE *stream);
void ImportTree          (Tree *tree, FILE *txtfile, unsigned int line);
void ReadNode            (Tree* tree, Node* node, LeftOrRight LOR, Node* parent, const char * buffer, size_t *pos, unsigned int line);
char *ReadNodeName       (const char *buffer, size_t *pos);
size_t ReadToBuffer      (char *buffer, FILE *txtfile);

#endif 