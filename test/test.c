#include "canopy.h"
#include "picasso.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    canopy_window* win = canopy_create_window(800, 600, "Hello Canopy");
    BMP *bmp = picasso_load_bmp("background.bmp");
    if (!bmp) {
        fprintf(stderr, "Failed to load BMP\n");
        return 1;
    }

    printf("Loaded BMP: %dx%d\n", bmp->ih.width, bmp->ih.height);
    picasso_flip_buffer_vertical(bmp->image_data, bmp->ih.width, bmp->ih.height);

    canopy_render_bitmap(win, bmp->image_data,
                         bmp->ih.width, bmp->ih.height,
                         0, 0);


    while (!canopy_window_should_close(win)) {
        canopy_redraw_buffer(win);

        canopy_event event;
        while (canopy_poll_event(&event)) {
            switch (event.type) {
            case CANOPY_EVENT_MOUSE_DOWN:
                printf("mouse down\n");
                break;
            case CANOPY_EVENT_MOUSE_UP:
                printf("mouse up\n");
                break;
            case CANOPY_EVENT_MOUSE_MOVE:
                break;
            default:
                break;
            }
        }
    }

    canopy_destroy_window(win);
    free(bmp->image_data);
    free(bmp);

    return 0;
}

