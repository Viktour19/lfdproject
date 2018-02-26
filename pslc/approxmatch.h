#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int exactmatch(char** pattern, char** text);

//if k=0 exactmatch
int dynamicprogramming(char** pattern, char** text, int k);

int galilpark(char** pattern, char** text, int k);

int ukkonenwood(char** pattern, char** text, int k);

int boyermoore(char** pattern, char** text, int k);