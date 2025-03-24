// test.c
#include "canopy.h"
#include <stdio.h>

int main() {
    canopy_window* win = canopy_create_window(800, 600, "Hello Canopy");
    while (!canopy_window_should_close(win)) {

        canopy_event event;

        while(canopy_poll_event(&event))
        {
            switch(event.type){
            case CANOPY_EVENT_MOUSE_DOWN:
                printf("mouse down\n");
                break;
            case CANOPY_EVENT_MOUSE_UP:
                printf("mouse up\n");
                break;
            case CANOPY_EVENT_MOUSE_MOVE:
                printf("mouse move\n");
                break;
            }
        }

    }
    canopy_destroy_window(win);
    return 0;
}

