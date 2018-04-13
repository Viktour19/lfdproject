
#include "pslImplementation.h"
// #include "approxmatch.h"

int main(void)
{

    Event_t * events = NULL;
    events = (Event_t *) malloc(sizeof(Event_t));

    EventUnion e;

    e.action.deltaangle = 'z';
    e.action.deltaX = 'z';
    e.action.deltaY = 'z';
    e.action.deltaZ = 'z';
    e.action.grasp = 'z';

    push(events, e, 1);

    e.observation.diffZ = 'a';
    e.observation.diffY = 'a';
    e.observation.diffX = 'a';
    e.observation.diffangle = 'a';

    push(events, e, 2);

    e.action.deltaangle = 'b';
    e.action.deltaX = 'b';
    e.action.deltaY = 'b';
    e.action.deltaZ = 'b';
    e.action.grasp = 'b';

    push(events, e, 1);

    e.observation.diffZ = 'b';
    e.observation.diffY = 'b';
    e.observation.diffX = 'b';
    e.observation.diffangle = 'b';

    push(events, e, 2);

    e.action.deltaangle = 'z';
    e.action.deltaX = 'z';
    e.action.deltaY = 'z';
    e.action.deltaZ = 'z';
    e.action.grasp = 'z';

    push(events, e, 1);

    e.observation.diffZ = 'a';
    e.observation.diffY = 'a';
    e.observation.diffX = 'a';
    e.observation.diffangle = 'a';

    push(events, e, 2);

    e.action.deltaangle = 'b';
    e.action.deltaX = 'b';
    e.action.deltaY = 'b';
    e.action.deltaZ = 'b';
    e.action.grasp = 'b';

    push(events, e, 1);

    e.observation.diffZ = 'b';
    e.observation.diffY = 'b';
    e.observation.diffX = 'b';
    e.observation.diffangle = 'b';

    push(events, e, 2);

    e.action.deltaangle = 'z';
    e.action.deltaX = 'z';
    e.action.deltaY = 'z';
    e.action.deltaZ = 'z';
    e.action.grasp = 'z';

    push(events, e, 1);

    e.observation.diffZ = 'a';
    e.observation.diffY = 'a';
    e.observation.diffX = 'a';
    e.observation.diffangle = 'a';

    push(events, e, 2);

    e.action.deltaangle = 'b';
    e.action.deltaX = 'b';
    e.action.deltaY = 'b';
    e.action.deltaZ = 'b';
    e.action.grasp = 'b';

    push(events, e, 1);

    e.observation.diffZ = 'b';
    e.observation.diffY = 'b';
    e.observation.diffX = 'b';
    e.observation.diffangle = 'b';

    push(events, e, 2);
    
    train(events, 0, getEventSeqLen(events) - 1);

    print_hypotheses();

    Event_t * events_ = NULL;
    events_  =  (Event_t *) malloc(sizeof(Event_t));

    EventUnion e_;

    e_.action.deltaangle = 'z';
    e_.action.deltaX = 'z';
    e_.action.deltaY = 'z';
    e_.action.deltaZ = 'z';
    e_.action.grasp = 'z';

    push(events_, e_, 1);

    for(int i = 0; i< 6; i++)
    {
        Event_t * pred =  predict(events_);
        push(events_, pred->event, pred->eventtype);
    
    }

    print_list(events_);
    

    CharList_t * _a = NULL;
    CharList_t * _b = NULL;

    _a = (CharList_t *) malloc(sizeof(CharList_t));
    _b = (CharList_t *) malloc(sizeof(CharList_t));

    push_char_list(_a, 'c');
    push_char_list(_a, 'a');    
    push_char_list(_a, 'c');
    push_char_list(_a, 'd');

    push_char_list(_b, 'b');
    push_char_list(_b, 'c');    
    push_char_list(_b, 'b');
    push_char_list(_b, 'a');
    push_char_list(_b, 'c');
    push_char_list(_b, 'b');
    push_char_list(_b, 'b');
    push_char_list(_b, 'b');    

    // dynamicprogramming(_a, _b, 0);
    
    return 0;

}