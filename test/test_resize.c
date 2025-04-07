/*******************************************************************************************
*
*   CANOPY [Example] - Custom Framebuffer Rendering
*
*   Description:
*       Renders a solid-colored rectangle using a manually created framebuffer.
*       Demonstrates how to allocate, fill, and display a custom framebuffer using Canopy.
*
*   Controls:
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
    picasso_image *src = picasso_load_bmp("assets/tiles.bmp");
    picasso_image *dst2x = picasso_alloc_image(270, 66, 4);
    picasso_image *dst3x = picasso_alloc_image(405, 99, 4);

    picasso_copy(src, dst2x);
    picasso_copy(src, dst3x);

    canopy_init_timer();
    canopy_set_fps(60);
    //--------------------------------------------------------------------------------------
    int dir = 4;
    //
    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Input
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        if (canopy_should_render_frame()) {

            picasso_clear_backbuffer(bf);

            picasso_blit_bitmap(bf, dst2x->pixels, dst2x->width, dst2x->height,0,33);
            picasso_blit_bitmap(bf, dst3x->pixels, dst3x->width, dst3x->height,0,99);
            picasso_blit_bitmap(bf, src->pixels, src->width, src->height,0,0);

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

