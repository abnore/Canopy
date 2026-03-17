/*******************************************************************************************
*
*   CANOPY [Example] - Picasso Backbuffer Blitting
*
*   Description:
*       Renders one solid-colored rectangle and one semi-transparent using a custom graphics
*       library called Picasso. Also loading a bmp and displaying that, all under a transparent
*       circle. One red square outlined moving to test clipping and boundries, and one outlined
*       circle movin to test boundries and drawing.
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
    if (!init_log(false, true, true)) {
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


    picasso_image *image = picasso_load_bmp("assets/sample1.bmp");

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
    double mouse_x = 0;
    double mouse_y = 0;
    canopy_init_timer();
    canopy_set_fps(144);
    //--------------------------------------------------------------------------------------
    int dir = 4;
    ERROR("Testing red error message");
    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Input
        //----------------------------------------------------------------------------------
        canopy_event event;
        while (canopy_poll_event(&event))
        {
            switch(event.type)
            {
                case CANOPY_EVENT_KEY:
                    if (event.key.action == CANOPY_KEY_PRESS) {
                        TRACE("Key pressed: %s (code %d)", canopy_key_to_string(event.key.keycode), event.key.keycode);
                    }
                    if (event.key.keycode == CANOPY_KEY_Q)
                    {
                        INFO("Closing the window, because you pressed q");
                        canopy_set_window_should_close(win);
                    }
                    break;
                case CANOPY_EVENT_MOUSE:
                    if( event.type == CANOPY_EVENT_MOUSE )
                    {
                        mouse_x = event.mouse.x;
                        mouse_y = event.mouse.y;
                    }
                    break;
                default: break;
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
            if (ypos - 40 <= 0 || ypos + 40 >= HEIGHT)  vel_y = -vel_y;
            xpos += vel_x;
            ypos += vel_y;


            picasso_blit_bitmap(bf,image, (WIDTH-image->width)/2, (HEIGHT-image->height)/2);
            picasso_fill_rect(bf, &rect_blue, BLUE);
            picasso_draw_rect(bf, &rect_red,10, RED);
            picasso_fill_circle(bf,xpos, ypos, 40, YELLOW);
            picasso_draw_circle(bf, mouse_x, mouse_y, 10,5, SET_ALPHA(PINK,90));
            picasso_fill_rect(bf, &rect_green, SET_ALPHA(GREEN, 40));
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
