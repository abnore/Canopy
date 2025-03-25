#include "canopy.h"

#include "picasso.h"
#include <stdio.h>
#include <stdlib.h>

#define CANOPY_RED     ((color){0x7f, 0x20, 0x20, 0xff})
#define CANOPY_GREEN   ((color){0x20, 0x7f, 0x20, 0xff})
#define CANOPY_BLUE    ((color){0x20, 0x20, 0x7f, 0xff}) //RGBA

#define HEIGHT 800
#define WIDTH 600

int main(void)
{
    canopy_window* win = canopy_create_window(HEIGHT, WIDTH, "Hello Canopy");
    color pixels[HEIGHT*WIDTH];

    picasso_fill_canvas(pixels, HEIGHT, WIDTH, CANOPY_BLUE);

    BMP *bmp_example = picasso_load_bmp("sample.bmp");
    BMP *bmp_mine = picasso_load_bmp("background.bmp");

    if(bmp_example) picasso_flip_buffer_vertical(bmp_example->image_data, bmp_example->ih.width, bmp_example->ih.height);
    if(bmp_mine) picasso_flip_buffer_vertical(bmp_mine->image_data, bmp_mine->ih.width, bmp_mine->ih.height);

    canopy_render_bitmap(win, pixels, HEIGHT, WIDTH, 0, 0);
    canopy_render_bitmap(win, bmp_example->image_data, bmp_example->ih.width, bmp_example->ih.height, 0, 0);
    canopy_render_bitmap(win, bmp_mine->image_data, bmp_mine->ih.width, bmp_mine->ih.height, 0, 0);


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
            case CANOPY_EVENT_MOUSE_DRAG:
                printf("mouse drag\n");
                break;
            default:
                break;
            }
        }
    }

    canopy_destroy_window(win);
    free(bmp_mine->image_data);
    free(bmp_mine);
    free(bmp_example->image_data);
    free(bmp_example);

    return 0;
}

