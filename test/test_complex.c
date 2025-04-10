/*******************************************************************************************
*
*   CANOPY [Example] - Moving Bitmap (Bouncing Background)
*
*   Description:
*       Loads two bmp images using Picasso, and animates one of them bouncing across the screen.
*       Demonstrates event handling, bitmap blitting, and backbuffer swapping with Canopy.
*
*   Controls:
*       [Mouse Click / Drag / Scroll] - Prints mouse interactions
*       [Keyboard] - Logs key presses
*       [Close Window] - Exit application
*
********************************************************************************************/

#include "canopy.h"
#include "picasso.h"


#define WIDTH   800
#define HEIGHT  600

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (!init_log(false, true, true)) {
        WARN("Failed to setup logger");
    }

    // Test logging levels
    FATAL("Testing log");
    ERROR("Testing log");
    WARN("Testing log");
    INFO("Testing log");
    DEBUG("Testing log");
    TRACE("Testing log");

    canopy_window* win = canopy_create_window("Canopy + Picasso - Bouncing Bitmap",
                                                WIDTH, HEIGHT,                                                CANOPY_WINDOW_STYLE_TITLED |
                                                CANOPY_WINDOW_STYLE_CLOSABLE);
    canopy_set_icon("assets/icon.png");

    picasso_image* bmp_example = picasso_load_bmp("assets/sample.bmp");
    picasso_image* bmp_mine    = picasso_load_bmp("assets/background.bmp");

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);

    // Initial animation values
    int xpos = 100;
    int ypos = 100;
    int vel_x = 1;
    int vel_y = 1;

    int sprite_width  = bmp_mine->width;
    int sprite_height = bmp_mine->height;

    canopy_init_timer();
    canopy_set_fps(60);
    //--------------------------------------------------------------------------------------

    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Input
        //----------------------------------------------------------------------------------
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
            }
            else if (event.type == CANOPY_EVENT_KEY && event.key.action == CANOPY_KEY_PRESS) {
                TRACE("Key down %s (code: %d)", canopy_key_to_string(event.key.keycode), event.key.keycode);
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        if (canopy_should_render_frame())
        {
            picasso_clear_backbuffer(bf);

            // Bounce logic
            if (xpos <= 0 || xpos + sprite_width  >= WIDTH)  vel_x = -vel_x;
            if (ypos <= 0 || ypos + sprite_height >= HEIGHT) vel_y = -vel_y;
            xpos += vel_x;
            ypos += vel_y;

            // Blit bitmaps
            picasso_blit_bitmap(bf, bmp_example, 0, 0);
            picasso_blit_bitmap(bf, bmp_mine,    xpos, ypos);

            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // In order to get the entire history, it is important that the log
    // is the last thing to be shutdown
    shutdown_log();
    // **Everything after this point does not get logged**
    canopy_free_window(win);
    picasso_destroy_backbuffer(bf);

    if (bmp_example) {
        free(bmp_example->pixels);
        free(bmp_example);
    }

    if (bmp_mine) {
        free(bmp_mine->pixels);
        free(bmp_mine);
    }
    //--------------------------------------------------------------------------------------

    return 0;
}
