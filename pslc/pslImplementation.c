#include "pslImplementation.h"

void init()
{
    //initialize the hypotheses library.
    //instantiate MAXHYP hypotheses and allocate memory for the body and head of each.

    for (int i = 0; i < MAXHYP; i++)
    {
        hypotheses[i].hits = 1;
        hypotheses[i].id = -1;

        hypotheses[i].lhs = malloc(sizeof(Event_t));
        hypotheses[i].rhs = malloc(sizeof(Event_t));

        hypotheses[i].lhs->eventtype = 0;
        hypotheses[i].lhs->next = NULL;

        hypotheses[i].rhs->eventtype = 0;
        hypotheses[i].rhs->next = NULL;
    }
}

Hypothesis newHyp()
{
    Hypothesis hyp;

    hyp.id = -1;
    hyp.hits = 1;
    hyp.misses = 0;
    hyp.lhs =  malloc(sizeof(Event_t));
    hyp.rhs =  malloc(sizeof(Event_t));

    return hyp;
}

void print_list(Event_t *head)
{

    Event_t *current = head;

    while (current != NULL)
    {

        printf("%d\n", current->eventtype);
        current = current->next;
    }
}

void push(Event_t * head, EventUnion val, int eventtype)
{
    Event_t *current = head;
    
    if(current->next == NULL && current->eventtype == 0)
    {
        current->eventtype = eventtype;
        current->event = val;
        return;
    }

    while (current->next != NULL)
    {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = malloc(sizeof(Event_t));
    current->next->event = val;
    current->next->eventtype = eventtype;
    current->next->next = NULL;
}

int getEventSeqLen(Event_t *sequence)
{

    Event_t *head = sequence;
    int ct = 0;

    while (head != NULL)
    {
        head = head->next;
        ct++;
    }

    return ct;
}

Event_t * get_by_index(Event_t *head, int n)
{

    int i = 0;

    Event_t *current = head;
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

Event_t * subsequence(Event_t *sequence, int startIndex, int stopIndex)
{
    Event_t * sub = NULL;
    sub = malloc(sizeof(Event_t));

    if ((stopIndex > startIndex) && (getEventSeqLen(sequence) >= stopIndex))
    {
        int sz = stopIndex - startIndex ;
        Event_t * current = get_by_index(sequence, startIndex);

        for (int i = 0; i < sz; i++)
        {   
            push(sub, current->event, current->eventtype);
            current = current->next;
        }
    }

    return sub;
}

int getVtFactor(Hypothesis hyp)
{
    int l = getEventSeqLen(hyp.lhs);
    int f = floor(l * VTFACTOR);

    return max(l + 1, f);
}

Hypothesis grow(Event_t *sequence, Hypothesis parent)
{
    int slen = getEventSeqLen(sequence);

    hypotheses[hypothesisCount].lhs = subsequence(sequence, slen - getVtFactor(parent), slen);

    hypotheses[hypothesisCount].rhs = parent.rhs;

    hypotheses[hypothesisCount].id = hypothesisCount;

    hypothesisCount += 1;
    return hypotheses[hypothesisCount - 1];
}

Hypothesis grow_sub(Event_t *sequence, Event_t *parent)
{
    //if PSL failed to predict

    int slen = getEventSeqLen(sequence);

    Hypothesis h = newHyp();

    h.lhs = subsequence(sequence, slen - 1, slen); //add the last event sequence
    h.rhs = parent;

    hypotheses[hypothesisCount] = h;
    hypotheses[hypothesisCount].id = hypothesisCount;

    hypothesisCount += 1;
    return hypotheses[hypothesisCount - 1];
}

double conf(int hypIndex)
{
    return (double) (getEventSeqLen(hypotheses[hypIndex].lhs) * hypotheses[hypIndex].hits) / (double)(hypotheses[hypIndex].hits + hypotheses[hypIndex].misses);
}

int support(int hypIndex)
{
    return hypotheses[hypIndex].misses + hypotheses[hypIndex].hits;
}

void reward(int hypIndex, int value)
{
    if (value)
    {
        hypotheses[hypIndex].hits += value;
    }
}

void punish(int hypIndex, int value)
{
    //punish the hypothesis by increasing the value of its misses

    if (value)
    {
        hypotheses[hypIndex].misses += value;
    }
}

int hyp_approxmatch(Event_t *a, Event_t *b)
{

    CharList_t * _a = NULL;
    CharList_t * _b = NULL;

    _a = malloc(sizeof(CharList_t));
    _b = malloc(sizeof(CharList_t));

    Event_t *current = a;

    while (current != NULL)
    {
        if (current->eventtype == 1)
        {
            
            push_char_list(_a,  current->event.action.deltaX);
            push_char_list(_a,  current->event.action.deltaY);
            push_char_list(_a,  current->event.action.deltaZ);
            push_char_list(_a,  current->event.action.deltaangle);
            push_char_list(_a,  current->event.action.grasp);

        }
        else if (current->eventtype == 2)
        {
            push_char_list(_a, current->event.observation.diffX);
            push_char_list(_a, current->event.observation.diffY);
            push_char_list(_a, current->event.observation.diffZ);
            push_char_list(_a, current->event.observation.diffangle);

        }

        current = current->next;
    }

    current = b;

    while (current != NULL)
    {
        if (current->eventtype == 1)
        {
            push_char_list(_b,  current->event.action.deltaX);
            push_char_list(_b,  current->event.action.deltaY);
            push_char_list(_b,  current->event.action.deltaZ);
            push_char_list(_b,  current->event.action.deltaangle);
            push_char_list(_b,  current->event.action.grasp);

        }
        else if (current->eventtype == 2)
        {
            push_char_list(_b, current->event.observation.diffX);
            push_char_list(_b, current->event.observation.diffY);
            push_char_list(_b, current->event.observation.diffZ);
            push_char_list(_b, current->event.observation.diffangle);

        }

        current = current->next;
    }

    return approxmatch(_a, _b, 0, 0);
    
}

double hypMatch(int hypIndex, Event_t *sequence)
{
    //match the lhs of the hypothesis with a given sequence returning a certain confidence score
    //adjusted for the length of the both sequences

    int seqlen = getEventSeqLen(sequence);
    int hypLhsLen = getEventSeqLen(hypotheses[hypIndex].lhs);

    if (sequence)
    {
        if (seqlen == 0 || seqlen > hypLhsLen)
            return conf(hypIndex);

        else
        {
            int a = hyp_approxmatch(hypotheses[hypIndex].lhs, sequence);
            if (a == 0) //should have some heuristic
            {
                double z = conf(hypIndex);
                z = z / seqlen * hypLhsLen;
                return z;
            }
        }
    }

    else
        return 0;

    return 0;
}

Hypothesis selectHyp(Event_t *seq)
{
    double scores [hypothesisCount];
    
    for (int i = 0; i < hypothesisCount; i++)
    {
        scores[i] = hypMatch(i, seq);
    }
    int max = 0;
    int index = 0;
    for (int i = 0; i < hypothesisCount; i++)
    {
        if (max < scores[i])
        {
            max = scores[i];
            index = i;
        }
    }
    Hypothesis nullHyp = newHyp();

    return scores[index] > 0.0 ? hypotheses[index] : nullHyp;
}

void getConfScores(Event_t *sequence, double hs[])
{    
    if (hypothesisCount == 0)
        return;

    for (int i = 0; i < hypothesisCount; i++)
    {
        double a = hypMatch(i, sequence);
        hs[i] = a;
    }

}

void clearHypotheses()
{
    for (size_t i = 0; i < MAXHYP; i++)
    {
        hypotheses[i].id = -1;

        for (size_t j = 0; j < getEventSeqLen(hypotheses[i].lhs); j++)
        {
            hypotheses[i].lhs[j].eventtype = 0;
        }

        hypotheses[i].rhs->eventtype = 0;
        hypotheses[i].misses = 0;
        hypotheses[i].hits = 1;
    }
}

int eventcompare(Event_t *a, Event_t *b)
{
    if (a->eventtype != b->eventtype)
        return -1;
    if (a->eventtype == 1)
    {
        if (a->event.action.deltaangle == b->event.action.deltaangle &&
            a->event.action.deltaX == b->event.action.deltaX && a->event.action.deltaY == b->event.action.deltaY &&
            a->event.action.deltaZ == b->event.action.deltaZ)
            return 0;
        else
            return -1;
    }
    else
    {
        if (a->event.observation.diffangle == b->event.observation.diffangle &&
            a->event.observation.diffX == b->event.observation.diffX && a->event.observation.diffY == b->event.observation.diffY &&
            a->event.observation.diffZ == b->event.observation.diffZ)
            return 0;
        else
            return -1;
    }
}

void train(Event_t *sequence, int startIndex, int stopIndex)
{
    init();
    int seqlen = getEventSeqLen(sequence);

    if(seqlen < 2) return;

    if (seqlen <= stopIndex || startIndex >= seqlen || startIndex > stopIndex)
        return;

    if(startIndex == 0) startIndex = 1;

    for (int i = startIndex; i < stopIndex + 1; i++)
    {
        Event_t * sub = subsequence(sequence, 0, i);
        Event_t * t = subsequence(sequence, i, i+1);

        double hs [hypothesisCount];

        getConfScores(sub, hs); //gets a confidence score for every hypothesis

        int hs_sz =  hs_sz = sizeof(hs) / sizeof(double);

        Hypothesis maxh = newHyp();
        
        double maxc = -1.0;
        Hypothesis bestCorrect = newHyp();
 
        for (int j = 0; j < hs_sz; j++)
        {
            double conf = hs[j];

            if (conf > 0.0)
            {
                Hypothesis hyp = hypotheses[j];

                if (maxc < conf)
                {
                    maxc = conf;
                    maxh = hyp;
                }

                if (eventcompare(hyp.rhs, t) == 0)
                {
                    reward(j, 1);

                    if (bestCorrect.id == -1 || getEventSeqLen(bestCorrect.lhs) < getEventSeqLen(hyp.lhs))
                    {
                        bestCorrect = hyp;
                    }
                }
            }
        }

        //if the rhs sugessted doesn't match the next event in the sequence, punish the hypothesis

        if(maxh.id != -1)
        {
            if (eventcompare(maxh.rhs, t) != 0)
                punish(maxh.id, 1);
        }


        //psl learns only on failure
        //hypothesis library is updated in grow
        
        if (bestCorrect.id == -1)
            grow_sub(sub, t);
        else
            grow(sub, bestCorrect);

    }
}

Event_t *predict(Event_t *seq)
{
    Hypothesis h = selectHyp(seq);
    return h.rhs;
}


