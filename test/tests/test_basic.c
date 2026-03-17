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

#define WIDTH   400
#define HEIGHT  400
#define PIXELS  WIDTH*HEIGHT

// Phthalo Blue https://colors.artyclick.com/color-names-dictionary/color-names/phthalo-blue-color
// little endian, abgr (rgba backwards)
#define PHTALO_BLUE 0xff890f00
#define CANOPY_BLUE 0xffff0000

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (!init_log(false, true, true)) {
        WARN("Failed to initialize logger\n");
        return 1;
    }

    canopy_window* win = canopy_create_window("Canopy - Custom Framebuffer",
                                                WIDTH, HEIGHT,
                                                CANOPY_WINDOW_STYLE_TITLED |
                                                CANOPY_WINDOW_STYLE_CLOSABLE);
    //canopy_set_window_transparent(win, true);
    canopy_init_timer();
    canopy_set_fps(60); // default is 60

    framebuffer *fb = canopy_get_framebuffer(win);
    //--------------------------------------------------------------------------------------

    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Update
        //----------------------------------------------------------------------------------
        canopy_event event;
        while (canopy_poll_event(&event))
        {
            // Handle events (mouse, keyboard, etc.)
            //INFO("An event was triggered!");
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        if (canopy_should_render_frame())
        {
            // Fill the framebuffer with its color
            for (int i = 0; i < PIXELS; ++i) {
                fb->pixels[i] = 0xff2020ff;
            }

            for (int i = 0; i < PIXELS; ++i) {

                int alpha_inc = 128; //((i/10)%WIDTH);

                if(i >= PIXELS/2){
                    uint32_t pixel = CANOPY_BLUE;
                    fb->pixels[i] = (pixel & 0x00ffffff) | ((alpha_inc) << 24);
                }
//                else{
//                    uint32_t pixel = PHTALO_BLUE;
//                    fb.pixels[i] = (pixel & 0x00ffffff) | ((alpha_inc) << 24);
//                }
            }

            // Do other stuff graphicly here

            //canopy_swap_backbuffer(win,fb);    // Switch pointers of custom framebuffer
            canopy_present_buffer(win);         // Present on screen
        }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    canopy_free_window(win);
    shutdown_log();
    //--------------------------------------------------------------------------------------

    return 0;
}
