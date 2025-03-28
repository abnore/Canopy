/*******************************************************************************************
*
*   CANOPY [Example] - Picasso Backbuffer Blitting
*
*   Description:
*       Renders 6 solid-colored rectangles using a custom graphics library called Picasso.
*       Demonstrates how to fill, blit, and display a custom backbuffer using Canopy.
*
*   Controls:
*       [Close Window] - Exit application
*
********************************************************************************************/

#include "canopy.h"

#define WIDTH   800
#define HEIGHT  600

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (!init_log(NULL, true)) {
        WARN("Failed to initialize logger");
        return 1;
    }

    canopy_window* win = canopy_create_window(WIDTH, HEIGHT, "Canopy + Picasso - Blitting");
    canopy_set_icon("assets/icon.svg");

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    if (!bf) {
        ERROR("Failed to create backbuffer");
        return 1;
    }

    // Create and fill rectangles
    color rect1[200 * 200];
    color rect2[200 * 200];
    color rect3[200 * 200];
    color rect4[200 * 200];
    color rect5[200 * 200];
    color rect6[200 * 200];

    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect1 }, CANOPY_RED);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect2 }, CANOPY_BLUE);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect3 }, CANOPY_GREEN);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect4 }, CANOPY_PURPLE);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect5 }, CANOPY_TEAL);
    picasso_fill_backbuffer(&(picasso_backbuffer){ .width = 200, .height = 200, .pixels = (uint32_t*)rect6 }, CANOPY_CYAN);

    canopy_init_timer();
    canopy_set_fps(60);
    canopy_set_buffer_refresh_color(win, CANOPY_LIGHT_GRAY);
    //--------------------------------------------------------------------------------------

    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Input
        //----------------------------------------------------------------------------------
        canopy_event event;
        while (canopy_poll_event(&event)) {
            if (event.type == CANOPY_EVENT_KEY && event.key.action == CANOPY_KEY_PRESS) {
                TRACE("Key pressed: %s (code %d)", canopy_key_to_string(event.key.keycode), event.key.keycode);
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        if (canopy_should_render_frame()) {
            canopy_clear_buffer(win);
            picasso_clear_backbuffer(bf);

            picasso_blit_bitmap(bf, rect1, 200, 200, 100, 100);
            picasso_blit_bitmap(bf, rect2, 200, 200, 350, 150);
            picasso_blit_bitmap(bf, rect3, 200, 200, 400, 200);
            picasso_blit_bitmap(bf, rect4, 200, 200, 200, 200);
            picasso_blit_bitmap(bf, rect5, 200, 200, 500, 250);
            picasso_blit_bitmap(bf, rect6, 200, 200, 550, 300);

            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    shutdown_log();
    picasso_destroy_backbuffer(bf);
    canopy_free_window(win);
    //--------------------------------------------------------------------------------------

    return 0;
}
