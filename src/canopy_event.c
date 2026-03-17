#include "canopy.h"

static callback_key key_callback = NULL;
static callback_mouse mouse_callback = NULL;
static callback_text text_callback = NULL;

// FIFO ring buffer
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

void dispatch_events(Window *w)
{
    canopy_event e;
    while (poll_event(&e)) {
        switch (e.type) {
            case CANOPY_EVENT_NONE: break;
            case CANOPY_EVENT_TEXT:
                if (text_callback)
                    text_callback(w, &e.text);
                break;
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

void set_callback_key(callback_key cb) {
    key_callback = cb;
}

void set_callback_mouse(callback_mouse cb) {
    mouse_callback = cb;
}

void set_callback_text(callback_text cb) {
    text_callback = cb;
}
