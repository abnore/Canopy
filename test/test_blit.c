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
    //canopy_set_window_transparent(win, true);

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    if (!bf) {
        ERROR("Failed to create backbuffer");
        return 1;
    }
    picasso_rect rect_red = {
        .x = -100,
        .y = -100,
        .width = 200,
        .height = -200,
    };
    picasso_rect rect_blue = {
        .x = 0,
        .y = 0,
        .width = WIDTH/2,
        .height = HEIGHT/2,
    };
    picasso_rect rect_green = {
        .x = WIDTH,
        .y = HEIGHT,
        .width = -(WIDTH/2),
        .height = -(HEIGHT/2),
    };

    // Initial animation values
    int xpos = 100;
    int ypos = 100;
    int vel_x = 6;
    int vel_y = 4;
    canopy_init_timer();
    canopy_set_fps(60);
    //--------------------------------------------------------------------------------------
    int dir = 4;
    ERROR("Testing red error message");
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

            if(rect_red.x > 1000 || rect_red.x < -500) dir = -dir;
            rect_red.x  += dir;
            rect_red.y  += dir;

            // Bounce logic
            if (xpos - 40 <= 0 || xpos + 40  >= WIDTH)  vel_x = -vel_x;
            if (ypos - 40 <= 0 || ypos + 40 >= HEIGHT) vel_y = -vel_y;
            xpos += vel_x;
            ypos += vel_y;


            picasso_fill_rect(bf, &rect_blue, BLUE);
            picasso_draw_rect(bf, &rect_red,10, RED);
            picasso_fill_circle(bf,xpos, ypos, 40, YELLOW);
            picasso_draw_circle(bf, WIDTH/2, HEIGHT/2, 100,5, PINK);
            picasso_fill_rect(bf, &rect_green, SET_ALPHA(GREEN, 60));
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
