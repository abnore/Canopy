#ifndef CANOPY_H
#define CANOPY_H

#include "common.h"

typedef struct canopy_window    canopy_window;
typedef struct canopy_cursor    canopy_cursor;
typedef struct canopy_image     canopy_image;

/* Basic windowing */
canopy_window *canopy_create_window(int width,
        int height,
        const char *title
        /*monitor*/
        /*window *share*/);
void        canopy_free_window(canopy_window *window);
int         canopy_window_should_close(canopy_window *window);
void        canopy_set_window_should_close(canopy_window *window, int value);
const char* canopy_get_window_title(canopy_window *window);
void        canopy_set_window_title(canopy_window *window, const char* title);
void        canopy_get_window_pos(canopy_window *window, int *pos_x, int *pos_y);
void        canopy_set_window_pos(canopy_window *window, int pos_x, int pos_y);
void        canopy_poll_events(void);

/* Events */
const char* canopy_get_key_name(int key, int scancode);
int         canopy_get_key_scancode(int key);
int         canopy_get_key(canopy_window *window, int key);
int         canopy_get_mouse_button(canopy_window *window, int button);
void        canopy_get_cursor_pos(canopy_window *window, double *pos_x, double *pos_y);
void        canopy_set_cursor_pos(canopy_window *window, double pos_x, double pos_y);
canopy_cursor *canopy_create_cursor(int shape);
void        canopy_free_cursor(canopy_cursor *cursor);
void        canopy_set_cursor(canopy_window *window, canopy_cursor *cursor);


#endif // CANOPY_H
