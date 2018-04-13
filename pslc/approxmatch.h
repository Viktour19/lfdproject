#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct CharList
{
    char element;
    struct CharList * next;

} CharList_t;


int exactmatch(CharList_t * pattern, CharList_t * text);

//if k=0 exactmatch
int dynamicprogramming(CharList_t * pattern, CharList_t * text, int k);

int galilpark(char** pattern, char** text, int k);

int ukkonenwood(char** pattern, char** text, int k);

int boyermoore(char** pattern, char** text, int k);

void print_char_list(CharList_t * head);

int exactmatch_(CharList_t * pattern, CharList_t * text);

int getListLen(CharList_t *list);

void push_char_list(CharList_t * head, int val);

CharList_t * get_char_by_index(CharList_t *head, int n);

int approxmatch(CharList_t * pattern, CharList_t * text, int k, int type);