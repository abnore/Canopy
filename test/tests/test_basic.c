/*******************************************************************************
*
*   CANOPY [Example] - Custom Framebuffer Rendering
*
*   Description:
*       Renders a solid-colored rectangle using a manually created framebuffer.
*       Demonstrates how to allocate, fill, and display a custom framebuffer.
*       Assumes installation of Canopy and BlackBox
*
*   Controls:
*       [Close Window] - Exit application
*
*******************************************************************************/

#include <canopy.h>
#include <blackbox.h>

#define WIDTH   400
#define HEIGHT  400

// Phthalo Blue
// https://colors.artyclick.com/color-names-dictionary/color-names/phthalo-blue-color
// little endian, abgr (rgba backwards)
#define PHTALO_BLUE 0xff890f00
#define CANOPY_BLUE 0xffff0000

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------
    init_log(LOG_DEFAULT);

    Window* win = create_window("Canopy - Custom Framebuffer", WIDTH, HEIGHT,
                                CANOPY_WINDOW_STYLE_DEFAULT);

    /* get_framebuffer(win) would return the framebuffer to the window, allowing
     * direct manipulation. This is the other way, for situations where it is
     * best to finish creating the buffer, and swap.*/
    framebuffer fb;
    get_framebuffer_size(win, &fb.width, &fb.height);
    fb.pitch = fb.width * CANOPY_BYTES_PER_PIXEL;

    size_t buffer_size = fb.pitch * fb.height;
    fb.pixels = canopy_malloc(buffer_size);

    if (!fb.pixels) {
        FATAL("Failed to allocate framebuffer");
        return 1;
    }

    int pixels = fb.width * fb.height;
    double current_time, prev_time = get_time();

    // set_fps(60);
    //--------------------------------------------------------------------------
    // Main Game Loop
    while (!window_should_close(win))
    {
        // Update
        pump_messages();
        //----------------------------------------------------------------------
        /* `dispatch_events(win)` allows you to create custom callbacks for both
         * key, mouse and text events that handles everything. If not, doing it
         * manually also works */
        canopy_event event;
        while (poll_event(&event))
        {
            // Handle events (mouse, keyboard, etc.)
        }
        //----------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------


        if (should_render_frame())
        {
            current_time = get_time();
            double frames_per_sec = 1 / (current_time - prev_time);
            prev_time = current_time;
            // Fill the framebuffer with its clear color
            for (int i = 0; i < pixels; ++i) {
                if(i >= pixels/2){
                    fb.pixels[i] = CANOPY_BLUE;
                }
                else{
                    fb.pixels[i] = PHTALO_BLUE;
                }
            }

            // Do other stuff graphicly here


            printf("\rFPS: %.4f", frames_per_sec);
            swap_backbuffer(win,&fb);   // Switch pointers of custom framebuffer
            present_buffer(win);        // Present on screen
        }
        //----------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------
    free(fb.pixels);
    free_window(win);
    shutdown_log();
    //--------------------------------------------------------------------------

    return 0;
}
