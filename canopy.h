//===================================================================================
// Canopy - A Minimal Windowing & Input Library for macOS
//
// Author: Andreas Nore (github.com/abnore)
//===================================================================================

#ifndef CANOPY_H
#define CANOPY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef CUSTOM_ALLOCATOR
#include <stdlib.h>   // Fallback to standard allocator if custom not defined
#endif

#ifdef __cplusplus
extern "C" { // Prevents name mangling of functions
#endif

typedef enum {
    CANOPY_KEY_A = 0x00,
    CANOPY_KEY_B = 0x0B,
    CANOPY_KEY_C = 0x08,
    CANOPY_KEY_D = 0x02,
    CANOPY_KEY_E = 0x0E,
    CANOPY_KEY_F = 0x03,
    CANOPY_KEY_G = 0x05,
    CANOPY_KEY_H = 0x04,
    CANOPY_KEY_I = 0x22,
    CANOPY_KEY_J = 0x26,
    CANOPY_KEY_K = 0x28,
    CANOPY_KEY_L = 0x25,
    CANOPY_KEY_M = 0x2E,
    CANOPY_KEY_N = 0x2D,
    CANOPY_KEY_O = 0x1F,
    CANOPY_KEY_P = 0x23,
    CANOPY_KEY_Q = 0x0C,
    CANOPY_KEY_R = 0x0F,
    CANOPY_KEY_S = 0x01,
    CANOPY_KEY_T = 0x11,
    CANOPY_KEY_U = 0x20,
    CANOPY_KEY_V = 0x09,
    CANOPY_KEY_W = 0x0D,
    CANOPY_KEY_X = 0x07,
    CANOPY_KEY_Y = 0x10,
    CANOPY_KEY_Z = 0x06,
    CANOPY_KEY_AE = 0x27,
    CANOPY_KEY_OE = 0x29,
    CANOPY_KEY_AA = 0x21,

    CANOPY_KEY_NUMPAD0 = 0x52,
    CANOPY_KEY_NUMPAD1 = 0x53,
    CANOPY_KEY_NUMPAD2 = 0x54,
    CANOPY_KEY_NUMPAD3 = 0x55,
    CANOPY_KEY_NUMPAD4 = 0x56,
    CANOPY_KEY_NUMPAD5 = 0x57,
    CANOPY_KEY_NUMPAD6 = 0x58,
    CANOPY_KEY_NUMPAD7 = 0x59,
    CANOPY_KEY_NUMPAD8 = 0x5B,
    CANOPY_KEY_NUMPAD9 = 0x5C,

    CANOPY_KEY_1 = 0x12,
    CANOPY_KEY_2 = 0x13,
    CANOPY_KEY_3 = 0x14,
    CANOPY_KEY_4 = 0x15,
    CANOPY_KEY_5 = 0x17,
    CANOPY_KEY_6 = 0x16,
    CANOPY_KEY_7 = 0x1A,
    CANOPY_KEY_8 = 0x1C,
    CANOPY_KEY_9 = 0x19,
    CANOPY_KEY_0 = 0x1D,

    CANOPY_KEY_F1 = 0x7A,
    CANOPY_KEY_F2 = 0x78,
    CANOPY_KEY_F3 = 0x63,
    CANOPY_KEY_F4 = 0x76,
    CANOPY_KEY_F5 = 0x60,
    CANOPY_KEY_F6 = 0x61,
    CANOPY_KEY_F7 = 0x62,
    CANOPY_KEY_F8 = 0x64,
    CANOPY_KEY_F9 = 0x65,
    CANOPY_KEY_F10 = 0x6D,
    CANOPY_KEY_F11 = 0x67,
    CANOPY_KEY_F12 = 0x6F,
    CANOPY_KEY_F13 = 0x69, // PRINT
    CANOPY_KEY_F14 = 0x6B,
    CANOPY_KEY_F15 = 0x71,
    CANOPY_KEY_F16 = 0x6A,
    CANOPY_KEY_F17 = 0x40,
    CANOPY_KEY_F18 = 0x4F,
    CANOPY_KEY_F19 = 0x50,
    CANOPY_KEY_F20 = 0x5A,

    CANOPY_KEY_ESCAPE = 0x35,
    CANOPY_KEY_SPACE = 0x31,
    CANOPY_KEY_TAB = 0x30,
    CANOPY_KEY_RETURN = 0x24,
    CANOPY_KEY_ENTER = 0x4C,
    CANOPY_KEY_DELETE = 0x75,

    CANOPY_KEY_UP = 0x7E,
    CANOPY_KEY_DOWN = 0x7D,
    CANOPY_KEY_LEFT = 0x7B,
    CANOPY_KEY_RIGHT = 0x7C,

    CANOPY_KEY_PAGE_UP = 0x74,
    CANOPY_KEY_PAGE_DOWN = 0x79,
    CANOPY_KEY_PAGE_HOME = 0x73,
    CANOPY_KEY_PAGE_END = 0x77,

    CANOPY_KEY_LSHIFT = 0x38,
    CANOPY_KEY_RSHIFT = 0x3C,

    CANOPY_KEY_LCONTROL = 0x3B,
    CANOPY_KEY_RCONTROL = 0x3E,

    CANOPY_KEY_LOPTION = 0x3A, // alt/option
    CANOPY_KEY_ROPTION = 0x3D, // alt/option

    CANOPY_KEY_LCOMMAND = 0x37,
    CANOPY_KEY_RCOMMAND = 0x36,

    CANOPY_KEY_CAPS_LOCK = 0x39,
    CANOPY_KEY_FN = 0x3F,

    CANOPY_KEY_APOSTROPHE = 0x27,
    CANOPY_KEY_BACKSLASH = 0x2A,
    CANOPY_KEY_COMMA = 0x2B,
    CANOPY_KEY_EQUAL = 0x18,
    CANOPY_KEY_GRAVE = 0x32,
    CANOPY_KEY_LBRACKET = 0x21,
    CANOPY_KEY_MINUS = 0x1B,
    CANOPY_KEY_PERIOD = 0x2F,
    CANOPY_KEY_RBRACKET = 0x1E,
    CANOPY_KEY_SEMICOLON = 0x29,
    CANOPY_KEY_SLASH = 0x2C,
    CANOPY_KEY_MAX_KEYS = 0x0A,
    CANOPY_KEY_BACKSPACE = 0x33,
    CANOPY_KEY_INSERT = 0x72,
    CANOPY_KEY_AX_KEYS = 0x6E,
    CANOPY_KEY_UMLOCK = 0x47,

    CANOPY_KEY_ADD = 0x45,
    CANOPY_KEY_DECIMAL = 0x41,
    CANOPY_KEY_DIVIDE = 0x4B,
    CANOPY_KEY_NUMPAD_EQUAL = 0x51,
    CANOPY_KEY_MULTIPLY = 0x43,
    CANOPY_KEY_SUBTRACT = 0x4E,
} canopy_key;

/* Converts a keyboard key to a human readable string for printing. Useful for
 * debugging and logging. */
const char* key_to_string(canopy_key key);

typedef enum {
    CANOPY_MOUSE_BUTTON_LEFT,
    CANOPY_MOUSE_BUTTON_MIDDLE,
    CANOPY_MOUSE_BUTTON_RIGHT,
    CANOPY_MAX_MOUSE_BUTTONS
} canopy_mouse_button;

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
} window_style;


//------------------------------------------------------------------------------
// Memory Allocation
//------------------------------------------------------------------------------
/* Custom allocators or wrappers if not defined */
void *canopy_calloc(size_t count, size_t size);
void canopy_free(void *ptr);
void *canopy_malloc(size_t size);
void *canopy_realloc(void *ptr, size_t size);

//------------------------------------------------------------------------------
// Main Library
//------------------------------------------------------------------------------
/*
    The canopy_window, named Window, is the main actor in this library, acting
    as the bridge between C and Objective-C. In order to not expose the Obj-C
    pointers, as they should not be accessed from user C-code, we will either
    need to have nested structs (like GLFW which uses an opaque type to a
    struct inside the higher level object), or just have the opaque type here.

    This is a forward declaration — effectively saying:

        “Hey compiler, Window is a struct that exists somewhere.
        I don’t need to know what it looks like yet, just that it exists.”

    This allows:
        - Declaring pointers to it (Window*)
        - Passing it to, and returning it from, functions

    But you cannot:
        - Dereference it
        - Access its members directly
        - Allocate it directly (its size is unknown)

    In other words: it's opaque on purpose.
*/
//==============================================================================
typedef struct canopy_window Window;
//==============================================================================

#define CANOPY_BYTES_PER_PIXEL 4

typedef struct {
    uint32_t    *pixels;      // Pixel buffer (RGBA).
    int         width;        // Width in pixels.
    int         height;       // Height in pixels.
    int         pitch;        // Number of bytes per row (row stride)
} framebuffer;

/* Creates and shows a new window with the given title. Width and height are in
 * points (logical size). The backing framebuffer is created in pixels, scaled
 * to match the window’s content scale. Returns a pointer to the newly created
 * window, or NULL on failure. */
Window* create_window(char* title, int width, int height, window_style flags);
void set_icon(const char* filepath);
void free_window(Window* window);

/* Gets the window scale (1.0, 2.0 etc) and size in points */
double get_window_scale(Window *window);
void get_window_size(Window *window, int *w, int *h);

/* Attaches user data to the window object, allowing for callbacks to interact
 * with outside data, such as audio */
void set_window_user_data(Window *window, void *user_data);
void *get_window_user_data(Window *window);

/* Check if the window should close (e.g. user clicked the close button)
 * Set sends a request to close the window */
bool window_should_close(Window* window);
void set_window_should_close(Window *window);

bool is_window_opaque(Window *window);
void set_window_transparent(Window *window, bool enable);

/* Gets the framebuffer for the window, and its size in pixels */
framebuffer* get_framebuffer(Window* window);
void get_framebuffer_size(Window *window, int *width, int *height);

/* Presents the contents of the framebuffer to the window
 * Optionally you can swap the backbuffer with the framebuffer, allowing better
 * renders */
void present_buffer(Window* window);
void swap_backbuffer(Window* window, framebuffer* bf);

/*==============================================================================
 * The event system for Canopy.
 * Supports polling and pushing input events.
 */
#define CANOPY_MAX_EVENTS 64

/* Type of high-level events. */
typedef enum {
    CANOPY_EVENT_NONE,
    CANOPY_EVENT_MOUSE,
    CANOPY_EVENT_KEY,
    CANOPY_EVENT_TEXT,
} canopy_event_type;

/* Mouse-specific event actions. */
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

/* Key-specific event actions. */
typedef enum {
    CANOPY_KEY_NONE,
    CANOPY_KEY_PRESS,
    CANOPY_KEY_RELEASE
} canopy_action_key;

typedef struct {
    char utf8[5]; // Enough for any UTF-8 encoded codepoint
} canopy_event_text;

/* Mouse event structure. */
typedef struct {
    canopy_action_mouse action;
    int x, y;
    canopy_mouse_button button;
    int modifiers;
    int click_count;
    float scroll_x;
    float scroll_y;
} canopy_event_mouse;

/* Keyboard event structure. */
typedef struct {
    canopy_action_key action;
    canopy_key keycode;
    int modifiers;
    int is_repeat;
} canopy_event_key;

/* Generic event union. Based on the type, one of these will contain the correct
 * information, and will be available as an event */
typedef struct {
    canopy_event_type type;
    union {
        canopy_event_mouse mouse;
        canopy_event_key key;
        canopy_event_text text;
    };
} canopy_event;


/* Callback functions to handle events. These must be user-defined, and set */
typedef void (*callback_key)(Window*, canopy_event_key*);
typedef void (*callback_mouse)(Window*, canopy_event_mouse*);
typedef void (*callback_text)(Window*, canopy_event_text*);

void set_callback_key(callback_key cb);
void set_callback_mouse(callback_mouse cb);
void set_callback_text(callback_text cb);

/* Fills in the mouse pos in the given x and y */
void get_mouse_pos(Window *window, double *x, double *y);

/* Allows callbacks to be setup, and when called handles events through the
 * user-defined callback. If callback are not set up, the alternative is to do
 * it manually with pump_events and then poll_event, acting on the data. */
void dispatch_events(Window *window);

/* Poll the next event, if available. out_event is a struct to fill,
 * Returns true with struct filled if available, false otherwise */
bool poll_event(canopy_event* out_event);
/* Process all pending events, and dispatch them to the internal event system.
 * This should be called regularly if using a manual event loop. */
void pump_events(void);

/* Posting a fake event to the queue to wake up wait-based event loops. This
 * can be useful to break out of wait_events() from another thread */
void post_empty_event(void);

/* Block until an event occurs and dispath it. Uses [NSDate distandFuture] to
 * sleep until the next input event. */
void wait_events(void);
/* Block until an event occurs or the timeout is reached. */
void wait_events_timeout(double timeout_seconds);

/* Pushed an event on the event-queue. Should not be touched directly, but
 * allows for "fake" events to be queued for the user */
void push_event(canopy_event event);

//------------------------------------------------------------------------------
// Timer Section
//------------------------------------------------------------------------------
/* Initializes the internal timer state
 * Must be called before any other timer-related functions */
void init_timer(void);

/* Returns the current monotonic time get_time() returns seconds as a double
 * get_time_ns() returns nanoseconds as an unsigned 64-bit integer */
double get_time(void);
uint64_t get_time_ns(void);

/* Returns the raw elapsed time, in seconds, between the last two accepted
 * frames. This can be used directly for time-based animation, or normalized;
 * usually by multiplying by FPS. */
double get_delta_time(void);

/* Sets or gets the target frames per second used by should_render_frame()
 * Defaults to 60 FPS, and values below 1 are clamped to 1 */
void set_fps(int fps);
int get_fps(void);

/* Checks whether enough time has passed to render the next frame
 * Returns 1 if a frame should be rendered, 0 otherwise. When it returns 1,
 * internal frame timing and delta time are updated */
int should_render_frame(void);

#ifdef __cplusplus
}
#endif
#endif // CANOPY_H
