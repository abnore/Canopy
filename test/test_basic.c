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

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (!init_log(NULL, true)) {
        WARN("Failed to initialize logger\n");
        return 1;
    }

    canopy_window* win = canopy_create_window(WIDTH, HEIGHT, "Canopy - Custom Framebuffer");
    canopy_set_buffer_refresh_color(win, CANOPY_DARK_GRAY);

    canopy_init_timer();
    canopy_set_fps(60);

    framebuffer fb;
    fb.width = 200;
    fb.height = 200;
    fb.pitch = fb.width * CANOPY_BYTES_PER_PIXEL;
    fb.clear_color = CANOPY_BLUE;

    size_t buffer_size = fb.pitch * fb.height;
    fb.pixels = canopy_malloc(buffer_size);

    if (!fb.pixels) {
        FATAL("Failed to allocate framebuffer");
        return 1;
    }

    // Fill the framebuffer with its clear color (once)
    for (int i = 0; i < fb.width * fb.height; ++i) {
        fb.pixels[i] = color_to_u32(fb.clear_color);
    }
    //--------------------------------------------------------------------------------------

    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Update
        //----------------------------------------------------------------------------------
        canopy_event event;
        while (canopy_poll_event(&event)) {
            // Handle events (mouse, keyboard, etc.)
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        if (canopy_should_render_frame()) {
            canopy_clear_buffer(win);                    // Clear main window framebuffer
            canopy_swap_backbuffer(win, &fb);            // Copy custom framebuffer
            canopy_present_buffer(win);                  // Present on screen
        }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    canopy_free(fb.pixels);
    canopy_free_window(win);
    shutdown_log();
    //--------------------------------------------------------------------------------------

    return 0;
}
