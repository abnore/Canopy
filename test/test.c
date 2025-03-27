#include "canopy.h"
#include "logger.h"
#include "picasso.h"

#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 600
#define WIDTH 800

int main(void)
{
    if(init_log(NULL, true) == LOG_ERROR) WARN("log failed to initialize");

    canopy_window *win = canopy_create_window(WIDTH, HEIGHT, "Canopy Blend");
    canopy_set_icon("assets/icon.svg");
    color rect1[200*200];
    color rect2[200*200];
    color rect3[200*200];
//    picasso_fill_canvas(rect1, 200, 200, CANOPY_RED);
//    picasso_fill_canvas(rect2, 200, 200, CANOPY_BLUE);
//    picasso_fill_canvas(rect3, 200, 200, CANOPY_GREEN);

    color rect4[200*200];
    color rect5[200*200];
    color rect6[200*200];
//    picasso_fill_canvas(rect4, 200, 200, CANOPY_PURPLE);
//    picasso_fill_canvas(rect5, 200, 200, CANOPY_NAVY);
//    picasso_fill_canvas(rect6, 200, 200, CANOPY_GOLD);

    canopy_init_timer();
    canopy_set_fps(60);
    DEBUG("color is %s", color_to_string(CANOPY_NAVY));
    canopy_set_buffer_refresh_color(win, CANOPY_NAVY);
    while(!canopy_window_should_close(win))
    {

        canopy_event event;
        while (canopy_poll_event(&event)) {
            switch (event.type) {
                case CANOPY_EVENT_KEY:
                    if (event.key.action == CANOPY_KEY_PRESS) {
                        TRACE("key down %s code: %d  ", canopy_key_to_string(event.key.keycode),

                                event.key.keycode);
                    } else if (event.key.action == CANOPY_KEY_RELEASE) {
                        //TRACE("key up %d", event.key.keycode);
                    }
                    break;
                default:
                    break;

            }
        }

        if (canopy_should_render_frame()) {
            canopy_clear_buffer(win);

            canopy_present_buffer(win);
        }
    }
    shutdown_log();
    canopy_free_window(win);
    return 0;
}

/*
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
    canopy_set_icon("assets/icon.png");
    BMP *bmp_example = picasso_load_bmp("sample.bmp");
    BMP *bmp_mine = picasso_load_bmp("background.bmp");
    BMP *bmp_tile = picasso_load_bmp("assets/tiles.bmp");

    if(bmp_example) picasso_flip_buffer_vertical(bmp_example->image_data, bmp_example->ih.width, bmp_example->ih.height);
    if(bmp_mine) picasso_flip_buffer_vertical(bmp_mine->image_data, bmp_mine->ih.width, bmp_mine->ih.height);
    if(bmp_tile) picasso_flip_buffer_vertical(bmp_tile->image_data, bmp_tile->ih.width, bmp_tile->ih.height);

    int minepos = 700;
    int xpos = 100;
    int ypos = 100;

    canopy_set_buffer_refresh_color(win, CANOPY_PURPLE);
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
                case CANOPY_EVENT_MOUSE:
                    switch (event.mouse.action) {
                        case CANOPY_MOUSE_PRESS:
                            TRACE("mouse press (button %d) at (%d, %d)",
                                    event.mouse.button, event.mouse.x, event.mouse.y);
                            break;
                        case CANOPY_MOUSE_RELEASE:
                            TRACE("mouse release (button %d) at (%d, %d)",
                                    event.mouse.button, event.mouse.x, event.mouse.y);
                            break;
                        case CANOPY_MOUSE_MOVE:
                            // TRACE("mouse move");
                            break;
                        case CANOPY_MOUSE_DRAG:
                            TRACE("mouse drag at (%d, %d)",
                                    event.mouse.x, event.mouse.y);
                            break;
                        case CANOPY_MOUSE_SCROLL:
                            TRACE("mouse scroll (x: %.2f, y: %.2f)",
                                    event.mouse.scroll_x, event.mouse.scroll_y);
                            break;
                        case CANOPY_MOUSE_ENTER:
                            TRACE("mouse entered window");
                            break;
                        case CANOPY_MOUSE_EXIT:
                            TRACE("mouse exited window");
                            break;
                        default:
                            break;
                    }
                    break;

                case CANOPY_EVENT_KEY:
                    if (event.key.action == CANOPY_KEY_PRESS) {
                        TRACE("key down %s code: %d  ", canopy_key_to_string(event.key.keycode),

                                                        event.key.keycode);
                    } else if (event.key.action == CANOPY_KEY_RELEASE) {
                        //TRACE("key up %d", event.key.keycode);
                    }
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
//            canopy_raster_bitmap(win, bmp_example->image_data, bmp_example->ih.width, bmp_example->ih.height, 0, 0);
//            canopy_raster_bitmap(win, bmp_mine->image_data, bmp_mine->ih.width, bmp_mine->ih.height, minepos, 0);
//            canopy_raster_bitmap_scaled(win, bmp_tile->image_data,
//                                        bmp_tile->ih.width, bmp_tile->ih.height,
//                                     xpos, ypos,
//                                        bmp_tile->ih.width*2, bmp_tile->ih.height*2);

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
*/
