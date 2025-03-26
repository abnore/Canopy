#ifndef CANOPY_H
#define CANOPY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "picasso.h"
#include "canopy_time.h"

typedef int WINDOW_STYLE_FLAGS;

enum {
        WindowStyleMaskBorderless = 0,
        WindowStyleMaskTitled = 1 << 0,
        WindowStyleMaskClosable = 1 << 1,
        WindowStyleMaskMiniaturizable = 1 << 2,
        WindowStyleMaskResizable = 1 << 3,
        WindowStyleMaskUnifiedTitleAndToolbar = 1 << 12,
        WindowStyleMaskFullScreen = 1 << 14,
        WindowStyleMaskFullSizeContentView = 1 << 15,
        WindowStyleMaskUtilityWindow = 1 << 4,
        WindowStyleMaskModalWindow = 1 << 6,
        WindowStyleMaskNonactivatingPanel = 1 << 7,
        WindowStyleMaskHUDWindow  = 1 << 13,
        WindowStyleDefault = 15
};

/* Safe wrappers */
void *canopy_calloc(size_t count, size_t size);
void canopy_free(void *ptr);
void *canopy_malloc(size_t size);
void *canopy_realloc(void *ptr, size_t size);

/* This is a forward declaration — I'm saying:

    “Hey compiler, canopy_window is a struct that exists somewhere.
    I don’t need to know what it looks like yet — just that it exists.”

    This allows:
        * Declaring pointers to it (canopy_window*)
        * Passing it around
        * Returning it from functions

    But you cannot dereference it, access members, or allocate it
    directly (you don’t know its size!).
*/
typedef struct canopy_window    canopy_window;
typedef struct canopy_event     canopy_event;

/* Basic Window Handling */
canopy_window* canopy_create_window(int width, int height, const char* title);
void canopy_free_window(canopy_window *w);
bool canopy_window_should_close(canopy_window *w);
void canopy_clear_buffer(canopy_window *w);
uint8_t *canopy_get_framebuffer(canopy_window *w);
void canopy_present_buffer(canopy_window *w);
void canopy_raster_bitmap(canopy_window *w, void* framebuffer, int width, int height, int x, int y);
void canopy_set_buffer_refresh_color(canopy_window *w, color c);




/* Basic Event Handling */

#define CANOPY_MAX_EVENTS 64

typedef enum {
    CANOPY_EVENT_NONE,
    CANOPY_EVENT_MOUSE_DOWN,
    CANOPY_EVENT_MOUSE_UP,
    CANOPY_EVENT_MOUSE_MOVE,
    CANOPY_EVENT_MOUSE_DRAG,
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
void canopy_push_event(canopy_event ev);

/* Not Implemented */
void        canopy_set_window_should_close(canopy_window *window, int value);
const char* canopy_get_window_title(canopy_window *window);
void        canopy_set_window_title(canopy_window *window, const char* title);
void        canopy_get_window_pos(canopy_window *window, int *pos_x, int *pos_y);
void        canopy_set_window_pos(canopy_window *window, int pos_x, int pos_y);
const char* canopy_get_key_name(int key, int scancode);
int         canopy_get_key_scancode(int key);
int         canopy_get_key(canopy_window *window, int key);
int         canopy_get_mouse_button(canopy_window *window, int button);
void        canopy_get_cursor_pos(canopy_window *window, double *pos_x, double *pos_y);

#ifdef __cplusplus
}
#endif

#endif // CANOPY_H
