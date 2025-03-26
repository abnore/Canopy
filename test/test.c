#include "canopy.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 600
#define WIDTH 800

int main(void)
{
    if(!init_log(NULL,true)) printf("Failed to setup logger\n");
    FATAL("Testing log");
    FATAL("Testing log");
    ERROR("Testing log");
    WARN("Testing log");
    INFO("Testing log");
    DEBUG("Testing log");
    TRACE("Testing log");

    canopy_window* win = canopy_create_window(WIDTH, HEIGHT, "Hello Canopy");
    BMP *bmp_example = picasso_load_bmp("sample.bmp");
    BMP *bmp_mine = picasso_load_bmp("background.bmp");
    BMP *bmp_tile = picasso_load_bmp("assets/tiles.bmp");

        if(bmp_example) picasso_flip_buffer_vertical(bmp_example->image_data, bmp_example->ih.width, bmp_example->ih.height);
    if(bmp_mine) picasso_flip_buffer_vertical(bmp_mine->image_data, bmp_mine->ih.width, bmp_mine->ih.height);
    if(bmp_tile) picasso_flip_buffer_vertical(bmp_tile->image_data, bmp_tile->ih.width, bmp_tile->ih.height);

    int minepos = 700;
    int xpos = 100;
    int ypos = 100;

    //canopy_set_buffer_refresh_color(win, CANOPY_GREEN);
    TRACE("first pixel = %08x\n", ((uint32_t*)canopy_get_framebuffer(win))[0]);

    canopy_init_timer();
    canopy_set_fps(60);

    int vel_x = 1;
    int vel_y = 1;

    int sprite_width = bmp_tile->ih.width;
    int sprite_height = bmp_tile->ih.height;

    while (!canopy_window_should_close(win)) {

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

            if (xpos <= 0 || xpos + sprite_width >= WIDTH) {
                vel_x = -vel_x;
            }
            if (ypos <= 0 || ypos + sprite_height >= HEIGHT) {
                vel_y = -vel_y;
            }

            xpos += vel_x;
            ypos += vel_y;
            canopy_clear_buffer(win);
            minepos--;
            //picasso_fill_canvas((color*)canopy_get_framebuffer(win), WIDTH, HEIGHT, CANOPY_BLUE);
            canopy_raster_bitmap(win, bmp_example->image_data, bmp_example->ih.width, bmp_example->ih.height, 0, 0);
            canopy_raster_bitmap(win, bmp_mine->image_data, bmp_mine->ih.width, bmp_mine->ih.height, minepos, 0);
            canopy_raster_bitmap(win, bmp_tile->image_data, bmp_tile->ih.width, bmp_tile->ih.height, xpos, ypos);

            canopy_present_buffer(win);
        }
    }

    shutdown_log();
    canopy_free_window(win);
    free(bmp_mine->image_data);
    free(bmp_mine);
    free(bmp_example->image_data);
    free(bmp_example);

    return 0;
}

