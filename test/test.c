#include "canopy.h"
#include "logger.h"
#include "picasso.h"


#define WIDTH 800
#define HEIGHT 600

#if 0
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
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect2 }, (color){0, 0, 200, 128});
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect3 }, (color){0, 200, 0, 170});
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect4 }, CANOPY_PURPLE);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect5 }, CANOPY_NAVY);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect6 }, CANOPY_GOLD);

    canopy_init_timer();
    canopy_set_fps(60);

    canopy_set_buffer_refresh_color(win, CANOPY_LIGHT_GRAY);

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
            picasso_clear_backbuffer(bf);
            // Blit the colored rectangles to the backbuffer
            picasso_blit_bitmap(bf, rect1, 200, 200, 100, 100 );
            picasso_blit_bitmap(bf, rect2, 200, 200, 150, 150 );
            picasso_blit_bitmap(bf, rect3, 200, 200, 200, 200 );
            picasso_blit_bitmap(bf, rect4, 200, 200, 300, 100 );
            picasso_blit_bitmap(bf, rect5, 200, 200, 350, 150 );
            picasso_blit_bitmap(bf, rect6, 200, 200, 400, 200 );

            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
    }

    shutdown_log();
    picasso_destroy_backbuffer(bf);
    canopy_free_window(win);
    return 0;
}

#else
int main(void)
{
    if (!init_log(NULL, true)) { printf("Failed to setup logger\n"); }

    FATAL("Testing log");
    ERROR("Testing log");
    WARN("Testing log");
    INFO("Testing log");
    DEBUG("Testing log");
    TRACE("Testing log");

    canopy_window* win = canopy_create_window(WIDTH, HEIGHT, "Hello Canopy");
    canopy_set_icon("assets/icon.png");

    BMP *bmp_example = picasso_load_bmp("assets/sample.bmp");
    BMP *bmp_mine = picasso_load_bmp("assets/background.bmp");
    BMP *bmp_tile = picasso_load_bmp("assets/tiles.bmp");

    if (bmp_example) picasso_flip_buffer_vertical(bmp_example->pixels, bmp_example->ih.width, bmp_example->ih.height);
    if (bmp_mine) picasso_flip_buffer_vertical(bmp_mine->pixels, bmp_mine->ih.width, bmp_mine->ih.height);
    if (bmp_tile) picasso_flip_buffer_vertical(bmp_tile->pixels, bmp_tile->ih.width, bmp_tile->ih.height);

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);

    int xpos = 100;
    int ypos = 100;
    int vel_x = 1;
    int vel_y = 1;

    int sprite_width = bmp_tile->ih.width;
    int sprite_height = bmp_tile->ih.height;

    canopy_set_buffer_refresh_color(win, CANOPY_NAVY);
    TRACE("First pixel = %08x", ((uint32_t*)canopy_get_framebuffer(win)->pixels)[0]);

    canopy_init_timer();
    canopy_set_fps(60);

    while (!canopy_window_should_close(win)) {

        canopy_event event;
        while (canopy_poll_event(&event)) {
            if (event.type == CANOPY_EVENT_MOUSE) {
                switch (event.mouse.action) {
                    case CANOPY_MOUSE_PRESS:
                        TRACE("Mouse press (button %d) at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
                        break;
                    case CANOPY_MOUSE_RELEASE:
                        TRACE("Mouse release (button %d) at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
                        break;
                    case CANOPY_MOUSE_DRAG:
                        TRACE("Mouse drag at (%d, %d)", event.mouse.x, event.mouse.y);
                        break;
                    case CANOPY_MOUSE_SCROLL:
                        TRACE("Mouse scroll (x: %.2f, y: %.2f)", event.mouse.scroll_x, event.mouse.scroll_y);
                        break;
                    case CANOPY_MOUSE_ENTER:
                        TRACE("Mouse entered window");
                        break;
                    case CANOPY_MOUSE_EXIT:
                        TRACE("Mouse exited window");
                        break;
                    default: break;
                }
            } else if (event.type == CANOPY_EVENT_KEY && event.key.action == CANOPY_KEY_PRESS) {
                TRACE("Key down %s (code: %d)", canopy_key_to_string(event.key.keycode), event.key.keycode);
            }
        }

        if (canopy_should_render_frame())
        {
            canopy_clear_buffer(win);

            if (xpos <= 0 || xpos + sprite_width >= WIDTH) vel_x = -vel_x;
            if (ypos <= 0 || ypos + sprite_height >= HEIGHT) vel_y = -vel_y;
            xpos += vel_x;
            ypos += vel_y;

            picasso_clear_backbuffer(bf);

            picasso_blit_bitmap(bf, bmp_example->pixels, bmp_example->ih.width, bmp_example->ih.height, 0, 0);
            picasso_blit_bitmap(bf, bmp_mine->pixels, bmp_mine->ih.width, bmp_mine->ih.height, 0, 0);
            picasso_blit_bitmap(bf, bmp_tile->pixels, bmp_tile->ih.width, bmp_tile->ih.height, xpos, ypos);


            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
    }

    shutdown_log();
    canopy_free_window(win);

    if (bmp_example) {
        free(bmp_example->pixels);
        free(bmp_example);
    }

    if (bmp_mine) {
        free(bmp_mine->pixels);
        free(bmp_mine);
    }

    if (bmp_tile) {
        free(bmp_tile->pixels);
        free(bmp_tile);
    }

    picasso_destroy_backbuffer(bf);
    return 0;
}
#endif
