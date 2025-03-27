#ifndef CANOPY_H
#define CANOPY_H

#ifdef __cplusplus
extern "C" { // Prevents name mangling of functions
#endif

#include "canopy_time.h"
#include "common.h"

/// @defgroup CanopyColors Predefined Canopy Color Macros
/// @{
#define CANOPY_BLUE         BLUE
#define CANOPY_GREEN        GREEN
#define CANOPY_RED          RED
#define CANOPY_WHITE        WHITE
#define CANOPY_BLACK        BLACK
#define CANOPY_GRAY         GRAY
#define CANOPY_LIGHT_GRAY   LIGHT_GRAY
#define CANOPY_DARK_GRAY    DARK_GRAY
#define CANOPY_ORANGE       ORANGE
#define CANOPY_YELLOW       YELLOW
#define CANOPY_BROWN        BROWN
#define CANOPY_GOLD         GOLD
#define CANOPY_CYAN         CYAN
#define CANOPY_MAGENTA      MAGENTA
#define CANOPY_PURPLE       PURPLE
#define CANOPY_NAVY         NAVY
#define CANOPY_TEAL         TEAL

#define CANOPY_BACKGROUND_COLOR CANOPY_DARK_GRAY
/// @}

/// @brief Bitmask flags for creating window styles.
typedef int WINDOW_STYLE_FLAGS;

/// @brief Available window style options for Canopy windows.
typedef enum {
    CANOPY_WINDOW_STYLE_BORDERLESS              = 0,
    CANOPY_WINDOW_STYLE_TITLED                  = 1 << 0,
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

    /// @brief Default window style (Titled, Closable, Miniaturizable, Resizable).
    CANOPY_WINDOW_STYLE_DEFAULT = CANOPY_WINDOW_STYLE_TITLED |
                                  CANOPY_WINDOW_STYLE_CLOSABLE |
                                  CANOPY_WINDOW_STYLE_MINIATURIZABLE |
                                  CANOPY_WINDOW_STYLE_RESIZABLE
} canopy_window_style;

/* This is a forward declaration — I'm saying:

    “Hey compiler, canopy_window is a struct that exists somewhere.
    I don’t need to know what it looks like yet — just that it exists.”

    This allows:
        * Declaring pointers to it (canopy_window*)
        * Passing it around and returning it from functions

    But you cannot dereference it, access members, or allocate it
    directly (you don’t know its size or contents !).
*/
/// @brief Opaque window structure. Don't access directly.
typedef struct canopy_window canopy_window;

/// @brief Bytes per pixel in the framebuffer (RGBA = 4 bytes)
#define CANOPY_BYTES_PER_PIXEL 4

/// @brief Framebuffer structure for storing pixel data.
typedef struct {
    uint32_t    *pixels;      ///< Pixel buffer (RGBA).
    int         width;        ///< Width in pixels.
    int         height;       ///< Height in pixels.
    int         pitch;        ///< Number of bytes per row.
    color       clear_color;  ///< Default clear color.
} framebuffer;

/// @name Memory Functions
/// @{
void* canopy_malloc(size_t size);
void* canopy_calloc(size_t count, size_t size);
void* canopy_realloc(void* ptr, size_t size);
void  canopy_free(void* ptr);
/// @}

/// @name Window Lifecycle
/// @{
canopy_window* canopy_create_window(int width, int height, const char* title);
void canopy_free_window(canopy_window* w);
void canopy_set_icon(const char* filepath);
bool canopy_window_should_close(canopy_window* w);
/// @}

/// @name Framebuffer Management
/// @{
bool canopy_init_framebuffer(canopy_window* win);
void canopy_clear_buffer(canopy_window* w);
framebuffer* canopy_get_framebuffer(canopy_window* w);
void canopy_present_buffer(canopy_window* w);
void canopy_swap_backbuffer(canopy_window* w, framebuffer* backbuffer);
void canopy_set_buffer_refresh_color(canopy_window* w, color c);
/// @}

/// @name Event Handling
/// @brief Supports polling and pushing input events.
/// @{
#define CANOPY_MAX_EVENTS 64

/// @brief Type of high-level events.
typedef enum {
    CANOPY_EVENT_NONE,
    CANOPY_EVENT_MOUSE,
    CANOPY_EVENT_KEY,
} canopy_event_type;

/// @brief Mouse-specific event actions.
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

/// @brief Key-specific event actions.
typedef enum {
    CANOPY_KEY_NONE,
    CANOPY_KEY_PRESS,
    CANOPY_KEY_RELEASE
} canopy_key_action;

/// @brief Mouse event structure.
typedef struct {
    canopy_mouse_action action;
    int x, y;
    mouse_buttons button;
    int modifiers;
    int click_count;
    float scroll_x;
    float scroll_y;
} canopy_mouse_event;

/// @brief Keyboard event structure.
typedef struct {
    canopy_key_action action;
    keys keycode;
    int modifiers;
    int is_repeat;
} canopy_key_event;

/// @brief Generic event union.
typedef struct {
    canopy_event_type type;
    union {
        canopy_mouse_event mouse;
        canopy_key_event key;
    };
} canopy_event;

/// @brief Poll the next event. Returns false if no events are available.
bool canopy_poll_event(canopy_event* out_event);

/// @brief Pump events into the internal event queue.
void canopy_pump_events(void);

/// @brief Manually push an event into the queue.
void canopy_push_event(canopy_event event);

/// @brief Get human-readable string for a key.
const char* canopy_key_to_string(keys key);
/// @}

/// @name (Optional) Window & Input Utilities
/// @{
void canopy_get_window_pos(canopy_window* window, int* pos_x, int* pos_y);
void canopy_set_window_pos(canopy_window* window, int pos_x, int pos_y);
const char* canopy_get_key_name(int key, int scancode);
int canopy_get_key_scancode(int key);
int canopy_get_key(canopy_window* window, int key);
int canopy_get_mouse_button(canopy_window* window, int button);
void canopy_get_cursor_pos(canopy_window* window, double* pos_x, double* pos_y);
/// @}


#ifdef __cplusplus
}
#endif
#endif // CANOPY_H

