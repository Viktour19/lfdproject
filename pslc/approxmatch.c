#include "approxmatch.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))


void print_char_list(CharList_t * head) {
    CharList_t * current = head;

    while (current != NULL) {
        printf("%c\n", current->element);
        current = current->next;
    }
}

void push_char_list(CharList_t * head, int val) {
    
    // int len = getListLen(head);

    CharList_t * current = head;

    while (current->next != NULL) {
            current = current->next;
    }

    /* now we can add a new variable */
    current->next = malloc(sizeof(CharList_t));
    current->element = val;
    current->next->next = NULL;
}

int getListLen(CharList_t *list)
{

    CharList_t *head = list;
    int ct = 0;

    while (head->next != NULL)
    {
        head = head->next;
        ct++;
    }

    return ct;
}

CharList_t * get_char_by_index(CharList_t *head, int n)
{

    int i = 0;

    CharList_t *current = head;
    if (n == 0)
    {
        return head;
    }

    for (i = 0; i < n; i++)
    {
        if (current->next == NULL)
        {
            return NULL;
        }
        current = current->next;
    }

    return current;
}

int approxmatch(CharList_t * pattern, CharList_t * text, int k, int type)
{

    if(k == 0)
    {
        return exactmatch(pattern, text);
    }

    return 0;
}

int exactmatch(CharList_t * pattern, CharList_t * text)
{

    CharList_t * p_current = pattern;
    CharList_t * t_current = text;

    int psz = getListLen(pattern);
    int tsz = getListLen(text);
    
    if(psz != tsz ) return -1;

    char p[psz];
    char t[tsz];

    int i = 0;

    while (p_current != NULL) {
        
        p[i++] = p_current->element;

        p_current = p_current->next;
    }

    i = 0;

    while (t_current != NULL) {
        
        t[i++] = t_current->element;

        t_current = t_current->next;
    }

    int a = strcmp(p, t);

    return a;
}

int dynamicprogramming(CharList_t * pattern, CharList_t * text, int k)
{
    int psz = getListLen(pattern);
    int tsz = getListLen(text);

    int D[psz +1][tsz +1];

    for(int j =0; j <= tsz; j++)
    {
        D[0][j] = 0;
    }

    for(int i =1; i <= psz; i++)
    {
        for(int j =0; j <= tsz; j++)
        {
            if(j < 1 )  D[i][j] = D[i-1][j] + 1;
            else 
            {
                D[i][j] = MIN( D[i-1][j] + 1 , D[i-1][j-1] + (get_char_by_index(pattern, i)->element == get_char_by_index(text, i)->element) ? 0 : 1);
                D[i][j] = MIN( D[i][j],  D[i][j - 1] + 1 );
            }

        }
    }

    for(int i = 0; i <= psz; i++)
    {
        for(int j =0; j <= tsz; j++)
        {
            printf("%d", D[i][j]);
        }

        printf("%s",  " ");

    }

    return 0;

}


