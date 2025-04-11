#include "canopy.h"

static canopy_callback_key key_callback = NULL;
static canopy_callback_mouse mouse_callback = NULL;

// FIFO ring buffer
static canopy_event event_queue[CANOPY_MAX_EVENTS];
static int event_head = 0;
static int event_tail = 0;

void canopy_push_event(canopy_event ev)
{
    int next = (event_tail + 1) % CANOPY_MAX_EVENTS;
    if (next != event_head) { // only add if queue not full
        event_queue[event_tail] = ev;
        event_tail = next;
    }
}

bool canopy_poll_event(canopy_event* out_event)
{
    if (event_head == event_tail) return false;
    *out_event = event_queue[event_head];
    event_head = (event_head + 1) % CANOPY_MAX_EVENTS;
    return true;
}

void canopy_dispatch_events(canopy_window *w)
{
    canopy_event e;
    while (canopy_poll_event(&e)) {
        switch (e.type) {
            case CANOPY_EVENT_NONE: break;
            case CANOPY_EVENT_KEY:
                if (key_callback)
                    key_callback(w, &e.key);
               break;
            case CANOPY_EVENT_MOUSE:
                if (mouse_callback)
                    mouse_callback(w, &e.mouse);
                break;
        }
    }
}
void canopy_set_callback_key(canopy_callback_key cb) {
    key_callback = cb;
}

void canopy_set_callback_mouse( canopy_callback_mouse cb) {
    mouse_callback = cb;
}
