//------------------------------------------------------------------------------
//
//  CANOPY [Example] - Most minimal setup
//
//  Description:
//      Opens a window, and demonstrates the required scaffolding to launch a
//      Canopy window
//
//  Controls:
//      [Close Window] - Exit application
//
//------------------------------------------------------------------------------

#include <canopy.h>
#include <blackbox.h>

int main(void)
{
    init_log(LOG_DEFAULT);

    Window* win = create_window("Canopy - Empty Window",
                                 600, 400, // Width and Height
                                 CANOPY_WINDOW_STYLE_TITLED |
                                 CANOPY_WINDOW_STYLE_CLOSABLE);
    // Main Loop
    //--------------------------------------------------------------------------
    while (!window_should_close(win))
    {
        // Update
        //----------------------------------------------------------------------
        pump_messages();
        dispatch_events(win);

        // Draw
        //----------------------------------------------------------------------
        if (should_render_frame())
        {
            present_buffer(win);
        }
        //----------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------
    free_window(win);
    shutdown_log();
    //--------------------------------------------------------------------------
    return 0;
}
