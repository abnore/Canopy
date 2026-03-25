#include "canopy.h"
#include "picasso.h"
#include <math.h>
#include <blackbox.h>

#define WIDTH   800
#define HEIGHT  600
#define CENTER_X (WIDTH / 2)
#define CENTER_Y (HEIGHT / 2)

static float angle = 0.0f;
static float _to_px_xf(const picasso_backbuffer *bf, float x) {
    return x * bf->scale_x;
}
static float _to_px_yf(const picasso_backbuffer *bf, float y) {
    return y * bf->scale_y;
}

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
    return (color){(uint8_t)(r * 255),
                   (uint8_t)(g * 255),
                   (uint8_t)(b * 255),
                          /* a */255};
}
// Drop in version from openAI - GPT 5.4. Checking out capabilities. Still crap
// rotates differently then what i want. I will try something else later.
// Leaving this here.
void picasso_fill_triangle_rainbow(picasso_backbuffer* bf,
                                   picasso_point3 t,
                                   float rotation)
{
    // Convert to float + scale
    float x1 = _to_px_xf(bf, t.x1);
    float y1 = _to_px_yf(bf, t.y1);
    float x2 = _to_px_xf(bf, t.x2);
    float y2 = _to_px_yf(bf, t.y2);
    float x3 = _to_px_xf(bf, t.x3);
    float y3 = _to_px_yf(bf, t.y3);

    // Bounding box (correct for floats)
    int min_x = (int)fmaxf(floorf(fminf(fminf(x1, x2), x3)), 0.0f);
    int max_x = (int)fminf(ceilf (fmaxf(fmaxf(x1, x2), x3)), bf->width  - 1);
    int min_y = (int)fmaxf(floorf(fminf(fminf(y1, y2), y3)), 0.0f);
    int max_y = (int)fminf(ceilf (fmaxf(fmaxf(y1, y2), y3)), bf->height - 1);

    // Triangle center (for your rainbow)
    float cx = (x1 + x2 + x3) / 3.0f;
    float cy = (y1 + y2 + y3) / 3.0f;

    // Edge function
    #define EDGE(ax, ay, bx, by, px, py) \
        ((px - ax)*(by - ay) - (py - ay)*(bx - ax))

    // Precompute area (for winding)
    float area = EDGE(x1, y1, x2, y2, x3, y3);

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {

            // Sample at pixel center (important!)
            float px = x + 0.5f;
            float py = y + 0.5f;

            float w0 = EDGE(x2, y2, x3, y3, px, py);
            float w1 = EDGE(x3, y3, x1, y1, px, py);
            float w2 = EDGE(x1, y1, x2, y2, px, py);

            // Inside test (same sign as area)
            if ((w0 >= 0 && w1 >= 0 && w2 >= 0 && area > 0) ||
                (w0 <= 0 && w1 <= 0 && w2 <= 0 && area < 0))
            {
                float dx = px - cx;
                float dy = py - cy;

                // rotate pixel BACK into triangle's local space
                float s = sinf(-rotation);
                float c = cosf(-rotation);

                float lx = dx * c - dy * s;
                float ly = dx * s + dy * c;

                float angle_local = atan2f(ly, lx);
                float hue = (angle_local + M_PI) / (2 * M_PI);

                // FIX: wrap properly
                hue = fmodf(hue, 1.0f);
                if (hue < 0.0f) hue += 1.0f;

                color rainbow = hsv_to_rgb(hue, 1.0f, 1.0f);
                bf->pixels[y * bf->width + x] = color_to_u32(rainbow);
            }
        }
    }

    #undef EDGE
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
    int w = bf->logical_width;
    int h = bf->logical_height;

    int center_x  = w / 2;
    int horizon_y = h / 3;
    int bottom_y  = h;

    int left_base  = -w;
    int right_base = 2 * w;

    // 1. Draw vertical perspective lines from bottom to vanishing point
    for (int x = left_base; x < right_base; x += spacing)
    {
        picasso_draw_line(bf, x, bottom_y, center_x, horizon_y, grid_color);
    }

    // 2. Draw horizontal lines clipped to the outer perspective bounds
    float z = 1.0f;
    float z_scale = 1.1f;

    while (1)
    {
        z *= z_scale;
        float y = (bottom_y - horizon_y) / z + horizon_y;
        if (y <= horizon_y)
            break;

        float t = (y - bottom_y) / (float)(horizon_y - bottom_y);

        int x_left  = (int)(left_base  + t * (center_x - left_base));
        int x_right = (int)(right_base + t * (center_x - right_base));

        picasso_draw_line(bf, x_left, (int)y, x_right, (int)y, grid_color);
    }
}
int main(void)
{
    init_log(LOG_DEFAULT);

    Window* win = create_window("Canopy + Picasso - Rainbow Triangle",
                                              WIDTH, HEIGHT,
                                              CANOPY_WINDOW_STYLE_TITLED |
                                              CANOPY_WINDOW_STYLE_CLOSABLE);
    set_icon("assets/picasso.png");
    set_fps(60);

    picasso_backbuffer* bf = picasso_create_backbuffer(win);

    // Base triangle to rotate
    picasso_point3 p = {220, 190, 600, 370, 260, 410};

    while (!window_should_close(win))
    {
        pump_messages();

        if (should_render_frame())
        {
            picasso_clear_backbuffer(bf);
            draw_perspective_grid(bf, 40, (color){0, 255, 255, 255});


            angle += 0.01f;
            picasso_point3 rot = rotate_triangle(p, angle);
            // Rainbow edge colors
            color red   = hsv_to_rgb(0.0f,       1.0f, 1.0f);  // Red
            color green = hsv_to_rgb(1.0f / 3.0f, 1.0f, 1.0f);  // Green
            color blue  = hsv_to_rgb(2.0f / 3.0f, 1.0f, 1.0f);  // Blue

            // Fill triangle with dark color
            //picasso_fill_triangle(bf, rot, (color){20, 20, 20, 255});
            //picasso_fill_triangle(bf, rot, RED);
            picasso_fill_triangle_rainbow(bf, rot, angle);

            // Draw anti-aliased rainbow edges
            picasso_draw_line_aa(bf, rot.x1, rot.y1, rot.x2, rot.y2, red);
            picasso_draw_line_aa(bf, rot.x2, rot.y2, rot.x3, rot.y3, green);
            picasso_draw_line_aa(bf, rot.x3, rot.y3, rot.x1, rot.y1, blue);

            swap_backbuffer(win, (framebuffer*)bf);
            present_buffer(win);
        }
    }

    free_window(win);
    picasso_destroy_backbuffer(bf);
    shutdown_log();

    return 0;
}
