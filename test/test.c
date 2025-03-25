#include "canopy.h"

#include <stdio.h>
#include <stdlib.h>

// RGBA
#define CANOPY_RED     ((color){0x7f, 0x20, 0x20, 0xff})
#define CANOPY_GREEN   ((color){0x20, 0x7f, 0x20, 0xff})
#define CANOPY_BLUE    ((color){0x20, 0x20, 0x7f, 0xff})
#define CANOPY_WHITE   ((color){0xff, 0xff, 0xff, 0xff})
#define CANOPY_BLACK   ((color){0x00, 0x00, 0x00, 0x00})

#define HEIGHT 600
#define WIDTH 800

int main(void)
{
    canopy_window* win = canopy_create_window(WIDTH, HEIGHT, "Hello Canopy");
    color pixels[HEIGHT*WIDTH];

    picasso_fill_canvas(pixels, WIDTH, HEIGHT, CANOPY_BLUE);

    BMP *bmp_example = picasso_load_bmp("sample.bmp");
    BMP *bmp_mine = picasso_load_bmp("background.bmp");
    BMP *bmp_tile = picasso_load_bmp("assets/tiles.bmp");

    if(bmp_example) picasso_flip_buffer_vertical(bmp_example->image_data, bmp_example->ih.width, bmp_example->ih.height);
    if(bmp_mine) picasso_flip_buffer_vertical(bmp_mine->image_data, bmp_mine->ih.width, bmp_mine->ih.height);
    if(bmp_tile) picasso_flip_buffer_vertical(bmp_tile->image_data, bmp_tile->ih.width, bmp_tile->ih.height);


    int position = 100;

    canopy_set_buffer_refresh_color(win, (color){200, 32, 32, 255});

    canopy_init_timer();
    canopy_set_fps(60);
//    const double target_frametime = 1.0 / 60.0; // 60 FPS = ~16.666ms per frame
//    clock_t last_frame = clock();

    while (!canopy_window_should_close(win)) {
//        clock_t now = clock();
//        double seconds_since_last = (double)(now - last_frame) / CLOCKS_PER_SEC;

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
        if (canopy_should_render_frame()) {
           // last_frame = now;

            position++;
            canopy_clear_buffer(win);

            // Render everything
            canopy_raster_bitmap(win, pixels, WIDTH, HEIGHT, 0, 0);
            canopy_raster_bitmap(win, bmp_example->image_data, bmp_example->ih.width, bmp_example->ih.height, 0, 0);
            canopy_raster_bitmap(win, bmp_mine->image_data, bmp_mine->ih.width, bmp_mine->ih.height, 0, 0);
            canopy_raster_bitmap(win, bmp_tile->image_data, bmp_tile->ih.width, bmp_tile->ih.height, WIDTH / 2, HEIGHT / 2);
            canopy_raster_bitmap(win, bmp_tile->image_data, bmp_tile->ih.width, bmp_tile->ih.height, position, position);

            canopy_present_buffer(win);
        }
    }

    canopy_free_window(win);
    free(bmp_mine->image_data);
    free(bmp_mine);
    free(bmp_example->image_data);
    free(bmp_example);

    return 0;
}

