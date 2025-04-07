/*******************************************************************************************
*
*   CANOPY [Example] - Resizing of images using Picasso library
*
*   Description:
*       Creates a picasso image from bmp and copies it.
*       Using blit on to the backbuffer and storing it as a bmp before displaying it
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
    if (!init_log(true, false, true)) {
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
    picasso_image *src = picasso_load_bmp("assets/sample1.bmp");
    int width = src->width, height = src->height;
    picasso_image *dst2x = picasso_alloc_image(width,height, 4);
    picasso_image *dst3x = picasso_alloc_image(width,height, 4);

    picasso_copy(src, dst2x);
    picasso_copy(src, dst3x);

    picasso_clear_backbuffer(bf);

    picasso_blit_bitmap(bf, dst2x->pixels, dst2x->width, dst2x->height,0,33);
    picasso_blit_bitmap(bf, dst3x->pixels, dst3x->width, dst3x->height,0,99);
    picasso_blit_bitmap(bf, src->pixels, src->width, src->height,0,0);

    picasso_image *file_from_bf = picasso_image_from_backbuffer(bf);
    bmp *file = picasso_create_bmp_from_rgba(file_from_bf->pixels, file_from_bf->width, file_from_bf->height, 4);
    picasso_save_to_bmp(file, "resize_new.bmp", 0);

    canopy_init_timer();
    canopy_set_fps(24);
    //--------------------------------------------------------------------------------------
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

