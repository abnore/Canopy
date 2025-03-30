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
#include "picasso.h" // for the colors and graphics

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

    canopy_window* win = canopy_create_window("Canopy + Picasso - Blitting",
                                                WIDTH, HEIGHT,
                                                CANOPY_WINDOW_STYLE_TITLED |
                                                CANOPY_WINDOW_STYLE_CLOSABLE);
    canopy_set_icon("assets/icon.svg");

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    if (!bf) {
        ERROR("Failed to create backbuffer");
        return 1;
    }
    picasso_rect rect_red = {
        .x = -100,
        .y = -200,
        .width = 200,
        .height = 200,
    };
    picasso_rect rect_blue = {
        .x = 450,
        .y = 150,
        .width = 100,
        .height = 100,
    };
    picasso_rect rect_green = {
        .x = 450,
        .y = 200,
        .width = -100,
        .height = -100,
    };

    color trans_green = GREEN;
    trans_green.a = 0x50;

    canopy_init_timer();
    canopy_set_fps(60);
    //--------------------------------------------------------------------------------------
    int dir = 1;
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

            picasso_clear_backbuffer(bf);

            if(rect_red.x > 800) dir = -dir;
            rect_red.x  += dir;
            rect_red.y  += dir;

            picasso_fill_rect(bf, &rect_red, RED);
            picasso_fill_rect(bf, &rect_blue, BLUE);
            picasso_fill_rect(bf, &rect_green, trans_green);

            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    picasso_destroy_backbuffer(bf);
    canopy_free_window(win);
    shutdown_log();
    //--------------------------------------------------------------------------------------

    return 0;
}
