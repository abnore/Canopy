//===================================================================================
// Minimal Windowing & Input Library for macOS
//
// Author: Andreas Nore (github@abnore)
//===================================================================================

#ifndef CANOPY_H
#define CANOPY_H

#ifdef __cplusplus
extern "C" { // Prevents name mangling of functions
#endif

#include "canopy_time.h"
#include "common.h"

// Available window style options for canopy windows.
typedef enum {
    CANOPY_WINDOW_STYLE_BORDERLESS              = 0,
    CANOPY_WINDOW_STYLE_TITLED                  = 1,
    CANOPY_WINDOW_STYLE_CLOSABLE                = 1 << 1,
    CANOPY_WINDOW_STYLE_MINIATURIZABLE          = 1 << 2,
    CANOPY_WINDOW_STYLE_RESIZABLE               = 1 << 3,
    CANOPY_WINDOW_STYLE_UTILITY                 = 1 << 4,
    CANOPY_WINDOW_STYLE_MODAL                   = 1 << 6,
    CANOPY_WINDOW_STYLE_NONACTIVATING_PANEL     = 1 << 7,
    CANOPY_WINDOW_STYLE_UNIFIED_TITLE_TOOLBAR   = 1 << 12,
    CANOPY_WINDOW_STYLE_HUD                     = 1 << 13,
    CANOPY_WINDOW_STYLE_FULLSCREEN              = 1 << 14,
    CANOPY_WINDOW_STYLE_FULLSIZE_CONTENT        = 1 << 15,

    CANOPY_WINDOW_STYLE_DEFAULT = CANOPY_WINDOW_STYLE_TITLED |
                                  CANOPY_WINDOW_STYLE_CLOSABLE |
                                  CANOPY_WINDOW_STYLE_MINIATURIZABLE |
                                  CANOPY_WINDOW_STYLE_RESIZABLE
}canopy_window_style;

/*
    The canopy_window is the main actor in this library,
    acting as the bridge between C and Objective-C.

    This is a forward declaration — effectively saying:

        “Hey compiler, canopy_window is a struct that exists somewhere.
        I don’t need to know what it looks like yet — just that it exists.”

    This allows:
        * Declaring pointers to it (canopy_window*)
        * Passing it to, and returning it from, functions

    But you cannot:
        * Dereference it
        * Access its members
        * Allocate it directly (its size is unknown)

    In other words: it's opaque on purpose. Treat it with respect.
    Don't access directly.
*/
//==============================================================================
typedef struct canopy_window canopy_window;
//==============================================================================

#define CANOPY_BYTES_PER_PIXEL 4

typedef struct {
    uint32_t    *pixels;      // Pixel buffer (RGBA).
    int         width;        // Width in pixels.
    int         height;       // Height in pixels.
    int         pitch;        // Number of bytes per row.
} framebuffer;


/*
 * Creates and shows a new window with the given size and title. Width and
 * height of the window in screen pixels. Returns a pointer to the newly
 * created window, or NULL on failure.
 */
canopy_window* canopy_create_window(const char* title,
                                    int width,
                                    int height,
                                    canopy_window_style flags);

void canopy_free_window(canopy_window* w);

/*
 * Set dock icon, give path to an image file to use as the application icon.
 */
void canopy_set_icon(const char* filepath);
bool canopy_is_window_opaque(canopy_window *win);
void canopy_set_window_transparent(canopy_window *w, bool enable);

/*
 * Send a request to close the window
 */
void canopy_set_window_should_close(canopy_window *w);

/*
 * Check if the window should close (user clocked the close button)
 * True if it is set to close, false otherwise
 */
bool canopy_window_should_close(canopy_window* w);

/*
 * Attaches a framebuffer to a window w
 */
bool canopy_init_framebuffer(canopy_window* w);
framebuffer* canopy_get_framebuffer(canopy_window* w);
/*
 * Presents the contents of the framebuffer to the window
 */
void canopy_present_buffer(canopy_window* w);
void canopy_swap_backbuffer(canopy_window* w, framebuffer* bf);

/*==============================================================================
 * The event system for Canopy.
 * Supports polling and pushing input events.
 */
#define CANOPY_MAX_EVENTS 64

// Type of high-level events.
typedef enum {
    CANOPY_EVENT_NONE,
    CANOPY_EVENT_MOUSE,
    CANOPY_EVENT_KEY,
    CANOPY_EVENT_TEXT,
} canopy_event_type;

// Mouse-specific event actions.
typedef enum {
    CANOPY_MOUSE_NONE,
    CANOPY_MOUSE_PRESS,
    CANOPY_MOUSE_RELEASE,
    CANOPY_MOUSE_MOVE,
    CANOPY_MOUSE_DRAG,
    CANOPY_MOUSE_SCROLL,
    CANOPY_MOUSE_ENTER,
    CANOPY_MOUSE_EXIT
} canopy_action_mouse;

// Key-specific event actions.
typedef enum {
    CANOPY_KEY_NONE,
    CANOPY_KEY_PRESS,
    CANOPY_KEY_RELEASE
} canopy_action_key;

typedef struct {
    char utf8[5]; // Enough for any UTF-8 encoded codepoint
} canopy_event_text;

// Mouse event structure.
typedef struct {
    canopy_action_mouse action;
    int x, y;
    mouse_buttons button;
    int modifiers;
    int click_count;
    float scroll_x;
    float scroll_y;
} canopy_event_mouse;

// Keyboard event structure.
typedef struct {
    canopy_action_key action;
    keys keycode;
    int modifiers;
    int is_repeat;
} canopy_event_key;

// Generic event union.
typedef struct {
    canopy_event_type type;
    union {
        canopy_event_mouse mouse;
        canopy_event_key key;
        canopy_event_text text;
    };
} canopy_event;

void canopy_get_mouse_pos(canopy_window *window, double *x, double *y);

// Callback functions to handle events
typedef void (*canopy_callback_key)(canopy_window*, canopy_event_key*);
typedef void (*canopy_callback_mouse)(canopy_window*, canopy_event_mouse*);
typedef void (*canopy_callback_text)(canopy_window*, canopy_event_text*);

void canopy_set_callback_key( canopy_callback_key cb);
void canopy_set_callback_mouse( canopy_callback_mouse cb);
void canopy_set_callback_text( canopy_callback_text cb);

/*
 * Poll the next event, if available. out_event is a struct to fill,
 * Returns true with strut filled if available, false otherwise
 */
bool canopy_poll_event(canopy_event* out_event);
void canopy_dispatch_events(canopy_window *w);

/* Posting a fake (empty) event to the queue to wake up wait-based event loops.
 * This can be useful to break out of canopy_wait_events() from another thread
 */
void canopy_post_empty_event(void);

/*
 * Process all pending events, and dispatch them to the internal event system.
 * This should be called regularly if using a manual event loop.
 */
void canopy_pump_events(void);

/*
 * Block until an event occurs and dispath it. Uses [NSDate distandFuture] to
 * sleep until the next input event.
 */
void canopy_wait_events(void);
/*
 * Block until an event occurs or the timeout is reached.
 */
void canopy_wait_events_timeout(double timeout_seconds);

void canopy_push_event(canopy_event event);



#ifdef __cplusplus
}
#endif
#endif // CANOPY_H

