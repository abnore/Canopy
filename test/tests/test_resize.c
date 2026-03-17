/*******************************************************************************
*
*   CANOPY [Example] - Resizing of images using Picasso library
*
*   Description:
*       Creates a picasso image from bmp and copies it. Using blit on to the
*       backbuffer and storing it as a bmp before displaying it
*
*   Controls:
*       [Close Window] - Exit application
*
*******************************************************************************/

#include "canopy.h"
#include "picasso.h"
#include <blackbox.h>

#define WIDTH   800
#define HEIGHT  600


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------
    if (!init_log(true, false, true)) {
        WARN("Failed to initialize logger");
        return 1;
    }
    Window* win = create_window("Canopy + Picasso - Blitting",
                                                WIDTH, HEIGHT,
                                                CANOPY_WINDOW_STYLE_TITLED |
                                                CANOPY_WINDOW_STYLE_CLOSABLE);
    set_icon("assets/icon.svg");
    //set_window_transparent(win, true);

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    if (!bf) {
        ERROR("Failed to create backbuffer");
        return 1;
    }
    picasso_image *tiles = picasso_load_bmp("assets/tiles.bmp");
    int width = tiles->width, height = tiles->height;
    picasso_image *dst2x = picasso_alloc_image(width*2,height*2, 4);
    picasso_image *dst3x = picasso_alloc_image(width*3,height*3, 4);

    picasso_copy(tiles, dst2x);
    picasso_copy(tiles, dst3x);

    picasso_clear_backbuffer(bf);

    picasso_blit_bitmap(bf, dst2x,0,33);
    picasso_blit_bitmap(bf, dst3x,0,99);
    picasso_blit_bitmap(bf, tiles,0,0);

    picasso_image *file_from_bf = picasso_image_from_backbuffer(bf);
    bmp *file = picasso_create_bmp_from_rgba(file_from_bf->pixels,
            file_from_bf->width, file_from_bf->height, 4);
    picasso_save_to_bmp(file, "resize_new.bmp", 0);

    init_timer();
    set_fps(24);
    //--------------------------------------------------------------------------
    // Main Game Loop
    while (!window_should_close(win))
    {
        // Input
        //----------------------------------------------------------------------
        picasso_rect tile_src = { 0, 0, 17, 17 };// get tile at (32,0) size 32x32
        picasso_rect draw_dst = { 500, 400, 96, 96 }; // draw scaled 2x2 on screen

        // Draw
        //----------------------------------------------------------------------
        if (should_render_frame()) {

            picasso_clear_backbuffer(bf);

            picasso_blit(bf, tiles, tile_src, draw_dst);
            picasso_blit_bitmap(bf, dst2x,0,33);
            picasso_blit_bitmap(bf, dst3x,0,99);
            picasso_blit_bitmap(bf, tiles,0,0);

            swap_backbuffer(win, (framebuffer*)bf);
            present_buffer(win);
        }
        //----------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------
    picasso_destroy_backbuffer(bf);
    free_window(win);
    shutdown_log();
    //--------------------------------------------------------------------------

    return 0;
}
