#include "canopy.h"
#include "logger.h"
#include "picasso.h"


#define WIDTH 800
#define HEIGHT 600

int main(void)
{
    if (init_log(NULL, true) == LOG_ERROR)
        WARN("Log failed to initialize");

    canopy_window *win = canopy_create_window(WIDTH, HEIGHT, "Canopy Blend");
    canopy_set_icon("assets/icon.svg");

    picasso_backbuffer *bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    if (!bf) {
        ERROR("Failed to create backbuffer");
        return 1;
    }

    color rect1[200 * 200];
    color rect2[200 * 200];
    color rect3[200 * 200];
    color rect4[200 * 200];
    color rect5[200 * 200];
    color rect6[200 * 200];

    // Fill the rectangles with colors
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect1 }, CANOPY_RED);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect2 }, CANOPY_BLUE);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect3 }, CANOPY_GREEN);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect4 }, CANOPY_PURPLE);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect5 }, CANOPY_NAVY);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect6 }, CANOPY_GOLD);

    canopy_init_timer();
    canopy_set_fps(60);
    canopy_set_buffer_refresh_color(win, CANOPY_NAVY);

    while (!canopy_window_should_close(win))
    {
        canopy_event event;
        while (canopy_poll_event(&event)) {
            if (event.type == CANOPY_EVENT_KEY && event.key.action == CANOPY_KEY_PRESS) {
                TRACE("Key pressed: %s (code %d)", canopy_key_to_string(event.key.keycode), event.key.keycode);
            }
        }

        if (canopy_should_render_frame()) {
            canopy_clear_buffer(win);

            // Blit the colored rectangles to the backbuffer
            picasso_blit_bitmap(bf, rect1, 200, 200, 100, 100, true);
            picasso_blit_bitmap(bf, rect2, 200, 200, 150, 150, true);
            picasso_blit_bitmap(bf, rect3, 200, 200, 200, 200, true);
            picasso_blit_bitmap(bf, rect4, 200, 200, 300, 100, true);
            picasso_blit_bitmap(bf, rect5, 200, 200, 350, 150, true);
            picasso_blit_bitmap(bf, rect6, 200, 200, 400, 200, true);

            // Extract the pixel buffer and pass to Canopy
            framebuffer temp_fb = {
                .pixels = picasso_backbuffer_pixels(bf),
                .width = bf->width,
                .height = bf->height,
                .pitch = bf->pitch
            };

            canopy_swap_backbuffer(win, &temp_fb);
            canopy_present_buffer(win);
        }
    }

    shutdown_log();
    picasso_destroy_backbuffer(bf);
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
