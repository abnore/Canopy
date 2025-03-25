#ifndef CANOPY_H
#define CANOPY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct canopy_window canopy_window;
typedef struct canopy_event canopy_event;

/* Basic Window Handling */
canopy_window* canopy_create_window(int width, int height, const char* title);
void canopy_destroy_window(canopy_window *window);

/* Basic Event Handling */

typedef enum {
    CANOPY_EVENT_NONE,
    CANOPY_EVENT_MOUSE_DOWN,
    CANOPY_EVENT_MOUSE_UP,
    CANOPY_EVENT_MOUSE_MOVE,
    CANOPY_EVENT_KEY_DOWN,
    CANOPY_EVENT_KEY_UP,
    CANOPY_EVENT_WINDOW_CLOSE
} canopy_event_type;

struct canopy_event {
    canopy_event_type type;

    union {
        struct {
            int key;
            int modifiers;
        } key;

        struct {
            int x;
            int y;
            int button;
            int modifiers;
        } mouse;
    };
};

bool canopy_poll_event(canopy_event *out_event);
void canopy_pump_events(void);
bool canopy_window_should_close(canopy_window *window);
void canopy_refresh_buffer(canopy_window *window);
void canopy_redraw_buffer(canopy_window *window);
void canopy_render_bitmap(canopy_window* win, void* buffer, int w, int h, int x, int y);

#ifdef __cplusplus
}
#endif

#endif // CANOPY_H

