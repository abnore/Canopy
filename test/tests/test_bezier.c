#include "canopy.h"
#include "picasso.h"

#include <math.h>

#define WIDTH 800
#define HEIGHT 600

picasso_vec2 control_point = {500, 200};  // Initial value

void mouse_callback(canopy_window *win, canopy_event_mouse *e) {
    (void)win;
    if (e->action == CANOPY_MOUSE_PRESS && e->button == CANOPY_MOUSE_BUTTON_LEFT) {
        control_point.x = e->x;
        control_point.y = e->y;
        TRACE("Control point moved to (%d, %d)", control_point.x, control_point.y);
    }
}

int main(void)
{
    init_log(NO_LOG, LOG_COLORS, STDERR_TO_LOG);
    canopy_init_timer();
    canopy_set_fps(24);
    canopy_window *win = canopy_create_window("Bezier", WIDTH, HEIGHT, CANOPY_WINDOW_STYLE_DEFAULT);
    canopy_set_callback_mouse(mouse_callback);
    picasso_backbuffer *bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    while(!canopy_window_should_close(win))
    {
        canopy_dispatch_events(win);
        picasso_vec2 p0 = {100, 400};
        picasso_vec2 p1 = control_point;
        picasso_vec2 p2 = {600, 400};
        // do your thing
        if (canopy_should_render_frame()){

            picasso_clear_backbuffer(bf);
            picasso_fill_circle_aa(bf, p0.x, p0.y, 5, WHITE);
            picasso_fill_circle_aa(bf, p1.x, p1.y, 5, WHITE);
            picasso_fill_circle_aa(bf, p2.x, p2.y, 5, WHITE);

            draw_bezier(bf, p0, p1, p2, 300);

            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
        //
    }
    canopy_free_window(win);
    shutdown_log();
    return 0;
}
