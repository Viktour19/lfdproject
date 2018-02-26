#include "approxmatch.h"


int exactmatch(char** pattern, char** text)
{
    int psz = sizeof(pattern)/sizeof(pattern[0]);
    int tsz = sizeof(text)/sizeof(text[0]);

    if(psz != tsz)
    {
        printf("%s", "sizes should match");
        return -1;
    }

    for(int i = 0; i< psz; i++)
    {
        while(strcmp(pattern[i], text[i]) !=0)
        {
            //failure to match
            return -1;
        }
    }

    return 0;

}


// int main()
// {
//     char** pattern;
//     char** text;

//     pattern = malloc(2 * sizeof(char*));
//     text = malloc(2 * sizeof(char*));
    
//     for (int i = 0; i < 2; i++)

//         pattern[i] = malloc((2) * sizeof(char));

//     *pattern = "ab";
//     *text = "ab";

//     int a = exactmatch(pattern, text);
    
//     printf("%d", a);

//     return 0;
// }

