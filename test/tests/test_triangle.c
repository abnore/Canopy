#include "canopy.h"
#include "picasso.h"
#include <math.h>

#define WIDTH   800
#define HEIGHT  600
#define CENTER_X (WIDTH / 2)
#define CENTER_Y (HEIGHT / 2)

static float angle = 0.0f;

// Basic HSV to RGB conversion
static color hsv_to_rgb(float h, float s, float v) {
    float r, g, b;
    int i = (int)(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }

    return (color){ (uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), 255 };
}
void picasso_fill_triangle_rainbow(picasso_backbuffer* bf, picasso_point3 t)
{
    // Compute triangle bounding box
    int min_x = PICASSO_CLAMP(PICASSO_MIN3(t.x1, t.x2, t.x3), 0, (int)bf->width - 1);
    int max_x = PICASSO_CLAMP(PICASSO_MAX3(t.x1, t.x2, t.x3), 0, (int)bf->width - 1);
    int min_y = PICASSO_CLAMP(PICASSO_MIN3(t.y1, t.y2, t.y3), 0, (int)bf->height - 1);
    int max_y = PICASSO_CLAMP(PICASSO_MAX3(t.y1, t.y2, t.y3), 0, (int)bf->height - 1);

    // Compute triangle center
    float cx = (t.x1 + t.x2 + t.x3) / 3.0f;
    float cy = (t.y1 + t.y2 + t.y3) / 3.0f;

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {

            // Barycentric coordinates
            int w0 = (t.x2 - t.x1)*(y - t.y1) - (t.y2 - t.y1)*(x - t.x1);
            int w1 = (t.x3 - t.x2)*(y - t.y2) - (t.y3 - t.y2)*(x - t.x2);
            int w2 = (t.x1 - t.x3)*(y - t.y3) - (t.y1 - t.y3)*(x - t.x3);

            bool has_neg = (w0 < 0) || (w1 < 0) || (w2 < 0);
            bool has_pos = (w0 > 0) || (w1 > 0) || (w2 > 0);
            if (has_neg && has_pos) continue;

            // Compute angle from center to pixel
            float dx = x - cx;
            float dy = y - cy;
            float angle = atan2f(dy, dx);   // [-π, π]
            float hue = (angle + M_PI) / (2 * M_PI);  // Normalize to [0, 1]

            color rainbow = hsv_to_rgb(hue, 1.0f, 1.0f);
            bf->pixels[y * bf->width + x] = color_to_u32(rainbow);
        }
    }
}
// Rotate triangle around its center
static picasso_point3 rotate_triangle(picasso_point3 p, float angle) {
    float cx = (p.x1 + p.x2 + p.x3) / 3.0f;
    float cy = (p.y1 + p.y2 + p.y3) / 3.0f;

    float s = sinf(angle);
    float c = cosf(angle);

    int pts[6] = { p.x1, p.y1, p.x2, p.y2, p.x3, p.y3 };

    for (int i = 0; i < 6; i += 2) {
        float x = pts[i] - cx;
        float y = pts[i + 1] - cy;
        pts[i]     = (int)(x * c - y * s + cx);
        pts[i + 1] = (int)(x * s + y * c + cy);
    }

    return (picasso_point3){ pts[0], pts[2], pts[4], pts[1], pts[3], pts[5] };
}
void draw_perspective_grid(picasso_backbuffer* bf, int spacing, color grid_color)
{
    int center_x = bf->width / 2;
    int horizon_y = bf->height / 3;
    int bottom_y = bf->height;

    // 1. Draw vertical perspective lines from bottom to vanishing point
    for (int x = -bf->width; x < bf->width * 2; x += spacing)
    {
        picasso_draw_line(bf, x, bottom_y, center_x, horizon_y, grid_color);
    }

    // 2. Draw horizontal lines spaced in perspective
    float z = 1.0f;
    float z_scale = 1.1f;
    float prev_y = (bottom_y - horizon_y) / z + horizon_y;

    while (prev_y > horizon_y )
    {
        z *= z_scale;
        float next_y = (bottom_y - horizon_y) / z + horizon_y;

        picasso_draw_line(bf, 0, (int)next_y, bf->width, (int)next_y, grid_color);
        prev_y = next_y;
    }
}

int main(void)
{
    if (!init_log(NO_LOG, LOG_COLORS, STDERR_TO_LOG)) {
        WARN("Failed to setup logger");
    }

    canopy_window* win = canopy_create_window("Canopy + Picasso - Rainbow Triangle",
                                              WIDTH, HEIGHT,
                                              CANOPY_WINDOW_STYLE_TITLED |
                                              CANOPY_WINDOW_STYLE_CLOSABLE);
    canopy_set_icon("assets/picasso.png");

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    canopy_init_timer();
    canopy_set_fps(60);

    // Base triangle to rotate
    picasso_point3 p = {220, 190, 600, 370, 260, 410};

    while (!canopy_window_should_close(win))
    {
        if (canopy_should_render_frame())
        {
            picasso_clear_backbuffer(bf);
            draw_perspective_grid(bf, 40, (color){0, 255, 255, 255});


            angle += 0.01f;
            picasso_point3 rotated = rotate_triangle(p, angle);
            // Rainbow edge colors
            color red   = hsv_to_rgb(0.0f,       1.0f, 1.0f);  // Red
            color green = hsv_to_rgb(1.0f / 3.0f, 1.0f, 1.0f);  // Green
            color blue  = hsv_to_rgb(2.0f / 3.0f, 1.0f, 1.0f);  // Blue

            // Fill triangle with dark color
             picasso_fill_triangle(bf, rotated, (color){20, 20, 20, 255});
            //picasso_fill_triangle(bf, rotated, RED);

            //picasso_fill_triangle(bf, rotated, RED);
            // picasso_fill_triangle_rainbow(bf, rotated);

            // Draw anti-aliased rainbow edges
            picasso_draw_line_aa(bf, rotated.x1, rotated.y1, rotated.x2, rotated.y2, red);
            picasso_draw_line_aa(bf, rotated.x2, rotated.y2, rotated.x3, rotated.y3, green);
            picasso_draw_line_aa(bf, rotated.x3, rotated.y3, rotated.x1, rotated.y1, blue);

            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
    }

    canopy_free_window(win);
    picasso_destroy_backbuffer(bf);
    shutdown_log();

    return 0;
}
