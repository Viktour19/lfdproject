
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "approxmatch.h"

#define EVENTLEN 27
#define MAXEVENT 200
#define MAXHYP 1000
#define VTFACTOR 2.0

// #define NULL ((void *)0)

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif


typedef struct {

    int id;
    char** lhs;
    char* rhs;
    int misses;
    int hits;

} Hypothesis;

int hypothesisCount = 0;
Hypothesis hypotheses[MAXHYP];


void init()
{
    for(int i= 0; i< MAXHYP; i++)
    {
        hypotheses[i].hits = 1;
        hypotheses[i].id = i+1;
    
        hypotheses[i].lhs = (char**) calloc(EVENTLEN, sizeof(char*));

        for (int j = 0; j < EVENTLEN; j++ )
        {
            hypotheses[i].lhs[j] = (char*) calloc(MAXEVENT, sizeof(char));
        }

        hypotheses[i].rhs = (char*) calloc(MAXEVENT, sizeof(char));
    }
}

char* strdup(const char* str)
{
      char* newstr = (char*) malloc( strlen( str) + 1);

      if (newstr) {
          strcpy( newstr, str);
      }

      return newstr;
}

int getEventSeqLen(char** sequence)
{
    if(sequence) return sizeof(sequence)/sizeof(sequence[0]);
    else return 0;
}

char** twodstrcpy(char** tosequence, char** fromsequence)
{
    int len = getEventSeqLen(fromsequence);

    for(int i =0; i< len; i++)
    {
        strncpy(tosequence[i], fromsequence[i], EVENTLEN);
    }

    return tosequence;
}

char** subsequence(char** sequence, int startIndex, int stopIndex)
{
    int sz = (stopIndex - startIndex ) + 1;

    char **sub = (char**) calloc(sz, sizeof(char*));

    for (int i = 0; i < sz; i++ )
    {
        sub[i] = (char*) calloc(MAXEVENT, sizeof(char));
    }

    for(size_t i =0; i < sz; i ++)
    {
         strcpy(sub[i], sequence[startIndex + i]);
    }

    return sub;
}

int getVtFactor(Hypothesis hyp)
{
    int l = getEventSeqLen(hyp.lhs);
    int f = floor(l * VTFACTOR);

    return max(l +1, f);

}

Hypothesis grow(char** sequence, Hypothesis parent)
{
    int slen = getEventSeqLen(sequence);

    twodstrcpy(hypotheses[hypothesisCount].lhs, subsequence(sequence, slen -  getVtFactor(parent), slen));
    strcpy(hypotheses[hypothesisCount].rhs, parent.rhs);
    
    hypothesisCount += 1;

    return hypotheses[hypothesisCount - 1];
}

Hypothesis grow_sub(char** sequence, char* parent)
{
    int slen = getEventSeqLen(sequence);

    Hypothesis h;
    twodstrcpy(h.lhs ,subsequence(sequence, slen-1, slen)); //add the last event sequence

    hypotheses[hypothesisCount] = h;
    
    hypothesisCount += 1;

    return hypotheses[hypothesisCount - 1];
}


int conf(Hypothesis hyp)
{
    return getEventSeqLen(hyp.lhs) * hyp.hits / (hyp.hits+hyp.misses);
}

int support(Hypothesis hyp)
{
    return hyp.misses + hyp.hits;
}

void reward(Hypothesis hyp, int value)
{
    if(value){
        hyp.hits += value;
    }
}

void punish(Hypothesis hyp, int value)
{
    if(value){
        hyp.misses += value;
    }
}


int approxmatch(char** a, char** b)
{
    return 1;
}

int hypMatch(Hypothesis hyp, char** sequence)
{
    int seqlen = getEventSeqLen(sequence);
    int hypLhsLen = getEventSeqLen(hyp.lhs);

    if(sequence)
    {
        if(seqlen == 0 || seqlen > hypLhsLen) return conf(hyp);
        
        else{
            
            // char subsequence[seqlen][EVENTLEN];
            char** subsequence = (char**) calloc(seqlen, sizeof(char*));

            for (int i = 0; i < seqlen; i++ )
            {
                subsequence[i] = (char*) calloc(EVENTLEN, sizeof(char));
                strcpy(subsequence[i], hyp.lhs[i]);
            }

            if(approxmatch(subsequence, sequence)) //should have some heuristic
            {
                return conf(hyp) / seqlen * hypLhsLen;
            }
        
        }
    } 
    else return 0;

}

void getConfScores(char** sequence, int* hs)
{   
    // int scores[hypothesisCount];
    int* scores = (int*) malloc(hypothesisCount);
    
    for(int i =0; i<hypothesisCount; i++)
    {
        scores[i] = hypMatch(hypotheses[i], sequence);
    }

    hs = scores;
}

void clearHypotheses()
{
    for(size_t i =0; i< MAXHYP; i++)
    {
        hypotheses[i].id = 0;
        
        for(size_t j = 0; j < getEventSeqLen(hypotheses[i].lhs); j++)
        {
            free(hypotheses[i].lhs[j]);            
        }

        free(hypotheses[i].rhs);

        hypotheses[i].misses = 0;
        hypotheses[i].hits = 1;

    } 
}

void train(char** sequence, int startIndex, int stopIndex)
{
    int seqlen = getEventSeqLen(sequence);

    if(seqlen<= stopIndex || startIndex >= seqlen || startIndex > stopIndex) return;

    for(int i =startIndex; i<stopIndex; i++)
    {
        char** sub = subsequence(sequence, 0, i);
        char* t = sequence[i];
        int* hs;
        getConfScores(sub, hs);
        int hs_sz = sizeof(hs);

        Hypothesis maxh;
		int maxc = -1;
		Hypothesis bestCorrect;
        
        for(int j = 0; j< hs_sz; j ++)
        {
            int conf =   hs[j];

            if(conf > 0)
            {
                Hypothesis hyp =  hypotheses[j];

                if(maxc < conf)
                {
                    maxc = conf;
                    maxh = hyp;
                }

                if(strcmp(hyp.rhs, t) == 0)
                {
                    reward(hyp, 1);

                    if(bestCorrect.id == 0 || getEventSeqLen(bestCorrect.lhs) < getEventSeqLen(hyp.lhs))
                    {
                        bestCorrect = hyp;
                    }
                }
            }
        }

        
        int correct = (strcmp(maxh.rhs, t) == 0);

        if(maxh.id != 0 && !correct) punish(maxh, 1);
        Hypothesis newh;
        if(correct)
        {
            if(bestCorrect.id == 0) newh = grow_sub(sub, t);
            else newh = grow(sub, bestCorrect);
        }
    }

}

