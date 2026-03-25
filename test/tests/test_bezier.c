#include "canopy.h"
#include "picasso.h"

#include <math.h>
#include <blackbox.h>

#define WIDTH 800
#define HEIGHT 600

picasso_vec2 control_point = {500, 200};  // Initial value

void mouse_callback(Window *win, canopy_event_mouse *e) {
    (void)win;
    if (e->action == CANOPY_MOUSE_PRESS && e->button == CANOPY_MOUSE_BUTTON_LEFT) {
        control_point.x = e->x;
        control_point.y = e->y;
        TRACE("Control point moved to (%d, %d)", control_point.x, control_point.y);
    }
}

int main(void)
{
    init_log(LOG_DEFAULT);

    set_fps(24);
    Window *win = create_window("Bezier", WIDTH, HEIGHT, CANOPY_WINDOW_STYLE_DEFAULT);
    set_callback_mouse(win, mouse_callback);
    picasso_backbuffer *bf = picasso_create_backbuffer(win);
    while(!window_should_close(win))
    {
        pump_messages();
        dispatch_events(win);
        picasso_vec2 p0 = {100, 400};
        picasso_vec2 p1 = control_point;
        picasso_vec2 p2 = {600, 400};
        // do your thing
        if (should_render_frame()){

            picasso_clear_backbuffer(bf);
            picasso_fill_circle_aa(bf, p0.x, p0.y, 5, WHITE);
            picasso_fill_circle_aa(bf, p1.x, p1.y, 5, WHITE);
            picasso_fill_circle_aa(bf, p2.x, p2.y, 5, WHITE);

            draw_bezier(bf, p0, p1, p2, 300);

            swap_backbuffer(win, (framebuffer*)bf);
            present_buffer(win);
        }
        //
    }
    free_window(win);
    shutdown_log();
    return 0;
}
