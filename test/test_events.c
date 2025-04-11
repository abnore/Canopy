/*******************************************************************************************
*
*   CANOPY [Example] - Canopy event handling
*
*   Description:
*       Defining the user-defined callbacks and seeing how the minimal functionality works.
*       Dispatching events instead of switching on them
*   Controls:
*       [Close Window] - Exit application
*
********************************************************************************************/

#include "canopy.h"
#include "picasso.h" // for the colors and graphics

#define WIDTH   800
#define HEIGHT  600

void handle_key(canopy_window *w, canopy_event_key* e )
{
    if (e->action == CANOPY_KEY_PRESS) {
        TRACE("Key pressed: %s (code %d) Modifier: %i", canopy_key_to_string(e->keycode),e->keycode, e->modifiers );
        if (e->keycode == CANOPY_KEY_Q)
        {
            INFO("Closing the window, because you pressed q");
            canopy_set_window_should_close(w);
        }
    }
}


void handle_mouse(canopy_window *w,  canopy_event_mouse* e )
{
    (void)w;
    if( e->action == CANOPY_MOUSE_MOVE ) {
        //INFO("Mouse moved to: (%.0d, %.0d)", e->x, e->y);
    } else if (e->action == CANOPY_MOUSE_PRESS) {

        INFO("CLICKED %s", e->button == CANOPY_MOUSE_BUTTON_LEFT? "left" : "right" );
    }
}
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (!init_log(false, true, true)) {
        WARN("Failed to initialize logger");
        return 1;
    }

    canopy_init_timer();
    canopy_set_fps(24);

    canopy_window* win = canopy_create_window("Canopy + Picasso - Blitting",
                                                WIDTH, HEIGHT,
                                                CANOPY_WINDOW_STYLE_TITLED |
                                                CANOPY_WINDOW_STYLE_CLOSABLE);
    canopy_set_icon("assets/icon.svg");

    canopy_set_callback_key( handle_key);
    canopy_set_callback_mouse( handle_mouse);

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    if (!bf) {
        ERROR("Failed to create backbuffer");
        return 1;
    }

    picasso_image *image = picasso_load_bmp("assets/sample1.bmp");
    //--------------------------------------------------------------------------------------
    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Input
        //----------------------------------------------------------------------------------
        canopy_dispatch_events(win);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        if (canopy_should_render_frame()) {

            picasso_clear_backbuffer(bf);
            picasso_blit_bitmap(bf,image, (WIDTH-image->width)/2, (HEIGHT-image->height)/2);
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
