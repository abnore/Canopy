/*******************************************************************************
*
*   CANOPY [Example] - Canopy event handling
*
*   Description:
*       Defining the user-defined callbacks and seeing how the minimal
*       functionality works. Dispatching events instead of switching on them
*   Controls:
*       [Close Window] - Exit application
*
*******************************************************************************/

#include "picasso.h" // for the colors and graphics

#include <canopy.h>
#include <blackbox.h>

#define WIDTH   800
#define HEIGHT  600

void swap_renderer(Renderer **a, Renderer **b) {
    Renderer *tmp = *a;
    *a = *b;
    *b = tmp;
}
void handle_text(Window *w, canopy_event_text *e) {
    char *buffer = get_window_user_data(w);
    static int string_cursor = 0;
    int i = 0;
    printf("\tTEXT:\"%s\"\n", e->utf8);
    fflush(stdout);

    while (e->utf8[i] != '\0') {
        buffer[string_cursor++] = e->utf8[i++];
    }

    buffer[string_cursor] = '\0';
}
void handle_key(Window *w, canopy_event_key* e )
{
    if (e->action == CANOPY_KEY_PRESS) {
        TRACE("Key pressed: %s (code %d) Modifier: %i",
                key_to_string(e->keycode),e->keycode, e->modifiers );
        if (e->keycode == CANOPY_KEY_Q)
        {
            INFO("Closing the window, because you pressed q");
            set_window_should_close(w);
        }
    }
}

void handle_mouse(Window *w,  canopy_event_mouse* e )
{
    (void)w;
    if( e->action == CANOPY_MOUSE_MOVE ) {
        //INFO("Mouse moved to: (%.0d, %.0d)", e->x, e->y);
    } else if (e->action == CANOPY_MOUSE_PRESS) {
    INFO("clicked %s", e->button == CANOPY_MOUSE_BUTTON_LEFT? "left" : "right");
    }
}
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------
    init_log(LOG_DEFAULT);

    set_fps(24);

    Window* win = create_window("Canopy + Picasso - Blitting",
                                                WIDTH, HEIGHT,
                                                CANOPY_WINDOW_STYLE_TITLED |
                                                CANOPY_WINDOW_STYLE_CLOSABLE);
    set_icon("assets/icon.svg");

    set_callback_key(win, handle_key);
    set_callback_mouse(win, handle_mouse);
    set_callback_text(win, handle_text);

    picasso_backbuffer *bf = picasso_create_backbuffer(win);

    if (!bf) {
        ERROR("Failed to create backbuffer");
        return 1;
    }

    Renderer *back = create_renderer(&bf->base);
    Renderer *front = create_renderer(get_framebuffer(win));

    Font *f = load_font("../fonts/LibreBaskerville-Regular.ttf");

       //      B      G      R    A
    Color c = {1.0f, 0.0f, 0.0f, 1.0f};
    double mouse_x, mouse_y;
    picasso_image *image = picasso_load_bmp("assets/sample1.bmp");

    //char string[5] = {0};
    char string[128] = {0};
    set_window_user_data(win, &string);
    //--------------------------------------------------------------------------
    // Main Game Loop
    while (!window_should_close(win))
    {
        pump_messages();
        // Input
        //----------------------------------------------------------------------
        dispatch_events(win);
        get_mouse_pos(win, &mouse_x, &mouse_y);
        //----------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------
        if (should_render_frame())
        {
            picasso_clear_backbuffer(bf);

            //advance = measure_text_width(f, string, 85.f);
            picasso_blit_bitmap(bf, image, (WIDTH-image->width)/2,
                                (HEIGHT-image->height)/2);

            draw_text(back, f, (const char*)&string, 0, 0, 85.f, c);
            picasso_fill_circle(bf, mouse_x, mouse_y, 5, RED);

            swap_backbuffer(win, &bf->base);
            swap_renderer(&back, &front);
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
