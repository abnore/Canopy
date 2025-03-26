#ifndef CANOPY_H
#define CANOPY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "picasso.h"
#include "canopy_time.h"
#include "logger.h"
#include "input.h"

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

/* Basic Window Handling */
canopy_window* canopy_create_window(int width, int height, const char* title);
void canopy_set_icon(const char* filepath);
void canopy_free_window(canopy_window *w);
bool canopy_window_should_close(canopy_window *w);
void canopy_clear_buffer(canopy_window *w);
uint8_t *canopy_get_framebuffer(canopy_window *w);
void canopy_present_buffer(canopy_window *w);
void canopy_set_buffer_refresh_color(canopy_window *w, color c);
void canopy_raster_bitmap_ex(canopy_window* win,
                             void* src_buf, int src_w, int src_h,
                             int dest_x, int dest_y,
                             int dest_w, int dest_h,
                             bool scale,
                             bool blend,
                             bool bilinear);
#define canopy_raster_bitmap(win, buf, w, h, x, y) \
    canopy_raster_bitmap_ex(win, buf, w, h, x, y, 0, 0, false, false, false)

#define canopy_raster_bitmap_scaled(win, buf, w, h, x, y, dw, dh) \
    canopy_raster_bitmap_ex(win, buf, w, h, x, y, dw, dh, true, false, false)

#define canopy_raster_bitmap_scaled_blended(win, buf, w, h, x, y, dw, dh) \
    canopy_raster_bitmap_ex(win, buf, w, h, x, y, dw, dh, true, true, false)


/* -------------------- Event Handling -------------------- */

#define CANOPY_MAX_EVENTS 64

//----------------------------------------
// Event Type Categories
//----------------------------------------
typedef enum {
    CANOPY_EVENT_NONE,
    CANOPY_EVENT_MOUSE,
    CANOPY_EVENT_KEY,
} canopy_event_type;

//----------------------------------------
// Mouse Sub-Actions
//----------------------------------------
typedef enum {
    CANOPY_MOUSE_NONE,
    CANOPY_MOUSE_PRESS,
    CANOPY_MOUSE_RELEASE,
    CANOPY_MOUSE_MOVE,
    CANOPY_MOUSE_DRAG,
    CANOPY_MOUSE_SCROLL,
    CANOPY_MOUSE_ENTER,
    CANOPY_MOUSE_EXIT
} canopy_mouse_action;

//----------------------------------------
// Keyboard Sub-Actions
//----------------------------------------
typedef enum {
    CANOPY_KEY_NONE,
    CANOPY_KEY_PRESS,
    CANOPY_KEY_RELEASE
} canopy_key_action;

//----------------------------------------
// Mouse Event Struct
//----------------------------------------
typedef struct {
    canopy_mouse_action action;
    int x, y;
    mouse_buttons button;        // 0 = left, 1 = right, 2 = middle
    int modifiers;     // CMD, SHIFT, etc
    int click_count;   // 1 = click, 2 = double-click
    float scroll_x;    // for scroll events
    float scroll_y;
} canopy_mouse_event;

//----------------------------------------
// Key Event Struct
//----------------------------------------
typedef struct {
    canopy_key_action action;
    keys keycode;
    int modifiers;
    int is_repeat;
} canopy_key_event;

//----------------------------------------
// Main Event Struct
//----------------------------------------
typedef struct {
    canopy_event_type type;

    union {
        canopy_mouse_event mouse;
        canopy_key_event   key;
    };
} canopy_event;


bool canopy_poll_event(canopy_event *out_event);
void canopy_pump_events(void);
void canopy_push_event(canopy_event event);

const char* canopy_key_to_string(keys key);

/* Not Implemented - dont know if i need them */
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
