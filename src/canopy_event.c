#include "canopy.h"
/* FIFO ring buffer without having linked lists. Using two pointers to head and
 * tail in a fixed array.
 * This is a global array for now, belonging to the entire library. This may be
 * best to assign to each window at some point, for now it stays like this */
static canopy_event event_queue[CANOPY_MAX_EVENTS];
static int event_head = 0;
static int event_tail = 0;

void push_event(canopy_event ev)
{
    int next = (event_tail + 1) % CANOPY_MAX_EVENTS;

    if (next != event_head) { // only add if queue not full
        event_queue[event_tail] = ev;
        event_tail = next;
    }
}

bool poll_event(canopy_event* out_event)
{
    if (event_head == event_tail) return false;

    *out_event = event_queue[event_head];
    event_head = (event_head + 1) % CANOPY_MAX_EVENTS;

    return true;
}
