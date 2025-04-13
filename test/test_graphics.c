/*******************************************************************************************
*
*   CANOPY [Example] - Line drawings and primitives
*
*   Description:
*       Draws lines and rects with and without antialiasing
*
********************************************************************************************/

#include "canopy.h"
#include "picasso.h"
#include <math.h>

#define WIDTH   800
#define HEIGHT  600


color hsv_to_rgb(float h, float s, float v)
{
    float r, g, b;

    int i = (int)(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return (color){
        .r = (uint8_t)(r * 255),
        .g = (uint8_t)(g * 255),
        .b = (uint8_t)(b * 255),
        .a = 255
    };
}
static inline uint32_t picasso__blend_pixel(uint32_t dst, uint32_t src)
{
    color back  = u32_to_color(dst);
    color front = u32_to_color(src);

    uint8_t sa = front.a;
    if (sa == 255) return src;
    if (sa == 0)   return dst;

    uint8_t r = (front.r * sa + back.r * (255 - sa)) / 255;
    uint8_t g = (front.g * sa + back.g * (255 - sa)) / 255;
    uint8_t b = (front.b * sa + back.b * (255 - sa)) / 255;
    uint8_t a = (front.a * 255 + back.a * (255 - sa)) / 255;

    color blended = { r, g, b, a };
    return color_to_u32(blended);
}
static inline void picasso__plot_aa(picasso_backbuffer *bf, int x, int y, color c, float alpha)
{
    if (x < 0 || x >= (int)bf->width || y < 0 || y >= (int)bf->height) return;

    c.a = (uint8_t)(c.a * alpha);
    uint32_t src = color_to_u32(c);
    uint32_t *dst_pixel = &bf->pixels[y * bf->width + x];
    *dst_pixel = picasso__blend_pixel(*dst_pixel, src);
}

void picasso_draw_rainbow_circle_aa(picasso_backbuffer *bf, int cx, int cy, int radius)
{
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            float dist2 = x * x + y * y;
            if (dist2 > radius * radius)
                continue;

            float angle = atan2f((float)y, (float)x); // -pi to pi
            float hue = (angle + M_PI) / (2.0f * M_PI); // normalize 0–1

            color rainbow = hsv_to_rgb(hue, 1.0f, 1.0f);
            picasso__plot_aa(bf, cx + x, cy + y, rainbow, 1.0f); // reuse AA pixel plot
        }
    }
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (!init_log(NO_LOG, LOG_COLORS, STDERR_TO_LOG)) {
        WARN("Failed to initialize logger");
        return 1;
    }
    canopy_window* win = canopy_create_window("Canopy + Picasso - Blitting",
                                                WIDTH, HEIGHT,
                                                CANOPY_WINDOW_STYLE_TITLED |
                                                CANOPY_WINDOW_STYLE_CLOSABLE);
    canopy_set_icon("assets/icon.svg");
    //canopy_set_window_transparent(win, true);

    picasso_backbuffer* bf = picasso_create_backbuffer(WIDTH, HEIGHT);
    if (!bf) {
        ERROR("Failed to create backbuffer");
        return 1;
    }

    double mouse_x, mouse_y;
//    float sin_y = 1, cos_x = 0;
//    float count=500;
//    float rate = 0.1f;
    canopy_init_timer();
    picasso_fill_rect(bf, &(picasso_rect){0,0,bf->width, bf->height},WHITE);
    picasso_draw_line_aa(bf, WIDTH/2, HEIGHT/2, 0,0, WHITE);
    picasso_draw_line_aa(bf, WIDTH/2, HEIGHT/2, WIDTH, HEIGHT, PINK);
    picasso_draw_line_aa(bf, WIDTH/2, HEIGHT, WIDTH/2, 0, PURPLE);
    picasso_draw_line_aa(bf, 0, HEIGHT/2, WIDTH, HEIGHT/2, GREEN);
    picasso_draw_line_aa(bf, WIDTH/3, 0, 2*WIDTH/3, HEIGHT, CYAN);
    //picasso_draw_circle_aa(bf, WIDTH/2,HEIGHT/2, 100, WHITE);
    picasso_fill_circle_aa(bf, WIDTH/2,HEIGHT/2, 150,RED);
    //picasso_draw_rainbow_circle_aa(bf, WIDTH/2,HEIGHT/2, 200);
    picasso_image *file_from_bf = picasso_image_from_backbuffer(bf);
    bmp *file = picasso_create_bmp_from_rgba(file_from_bf->pixels, file_from_bf->width, file_from_bf->height, 4);
    picasso_save_to_bmp(file, "graphics.bmp", 0);
    canopy_set_fps(24);
    //--------------------------------------------------------------------------------------
    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Input
        //----------------------------------------------------------------------------------
        canopy_get_mouse_pos(win, &mouse_x, &mouse_y);
        // Draw
        //----------------------------------------------------------------------------------
        if (canopy_should_render_frame()) {

            picasso_clear_backbuffer(bf);
            //            sin_y = sinf(count*rate)*50;
            //            cos_x = cosf(count*rate)*50;
            //            count++;
            //            picasso_draw_line(bf,0,0, WIDTH/2, HEIGHT/2,RED);
            //            picasso_fill_circle(bf, sin_y + WIDTH/2, cos_x+ HEIGHT/2 , 10, RED);

            picasso_draw_line_aa(bf, WIDTH/2, HEIGHT/2, 0,0, WHITE);
            picasso_draw_line_aa(bf, WIDTH/2, HEIGHT/2, WIDTH, HEIGHT, PINK);
            picasso_draw_line_aa(bf, WIDTH/2, HEIGHT, WIDTH/2, 0, PURPLE);
            picasso_draw_line_aa(bf, 0, HEIGHT/2, WIDTH, HEIGHT/2, GREEN);
            picasso_draw_line_aa(bf, WIDTH/3, 0, 2*WIDTH/3, HEIGHT, CYAN);
            //    picasso_draw_circle_aa(bf, WIDTH/2,HEIGHT/2, 100, WHITE);
            picasso_draw_rainbow_circle_aa(bf, WIDTH/2,HEIGHT/2, 200);
            //    picasso_fill_circle_aa(bf, WIDTH/2,HEIGHT/2, 100, WHITE);
            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    picasso_destroy_backbuffer(bf);
    canopy_free_window(win);
    shutdown_log();
    //--------------------------------------------------------------------------------------

    return 0;
}

