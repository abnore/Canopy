/*******************************************************************************************
*
*   CANOPY [Example] - Picasso Color conversion testing
*
*   Description:
*       Attempts to convert RGB to YCrCb - chrominance and luminance
*   Controls:
*       [Close Window] - Exit application
*
********************************************************************************************/
#include "picasso.h"
#include "canopy.h"

#include <math.h>

#define WIDTH 1200
#define HEIGHT 800

int main(void)
{
    if (!init_log(false, true, true)) {
        WARN("Failed to initialize logger");
        return 1;
    }

    canopy_window* win = canopy_create_window("Color conversion",
                                              WIDTH, HEIGHT,
                                              CANOPY_WINDOW_STYLE_TITLED |
                                              CANOPY_WINDOW_STYLE_CLOSABLE);

    picasso_backbuffer *bf = picasso_create_backbuffer(WIDTH, HEIGHT);

    picasso_image *bmp = picasso_load_bmp("assets/sample1.bmp");

    picasso_image *grayscale= picasso_alloc_image(bmp->width, bmp->height,1);
    picasso_image *red_chrominance= picasso_alloc_image(bmp->width, bmp->height, 4);
    picasso_image *blue_chrominance= picasso_alloc_image(bmp->width, bmp->height, 4);

    picasso_copy(bmp, grayscale);
    picasso_copy(bmp, red_chrominance);
    picasso_copy(bmp, blue_chrominance);

//    foreach_pixel_u8(grayscale, { // Handeled in copy now
//            color p = get_color_u8(pixel, grayscale->channels);
//            uint8_t Y = (76 * p.r + 150 * p.g + 29 * p.b) >> 8;
//           // float Y  = (0.299f * p.r) + (0.587f * p.g) + (0.114f * p.b);
//            uint8_t v = PICASSO_CLAMP(Y, 0, 255);
//            pixel = &v;
//    });
    foreach_pixel_u8(red_chrominance, {
            color p = get_color_u8(pixel, red_chrominance->channels);
            float Cr = 128.0f + (0.5f * p.r) - (0.418688f * p.g) - (0.081312f * p.b);
            uint8_t v = (uint8_t)PICASSO_CLAMP(Cr, 0, 255);
            pixel[0] = v;
            pixel[1] = pixel[2] = 0;
    });
    foreach_pixel_u8(blue_chrominance, {
            color p = get_color_u8(pixel, blue_chrominance->channels);
            float Cb = 128.0f - (0.168736f * p.r) - (0.331264f * p.g) + (0.5f * p.b);
            uint8_t v = (uint8_t)PICASSO_CLAMP(Cb, 0, 255);
            pixel[0] = pixel[1] = 0;
            pixel[2] = v;
    });


    canopy_init_timer();
    canopy_set_fps(24);

int SPACING = 10;

// Each quadrant is half the screen
int quad_width  = WIDTH / 2;
int quad_height = HEIGHT / 2;

// Available space inside each quadrant after spacing
int img_width  = quad_width - (SPACING*2);
int img_height = quad_height - (SPACING*2);
// TOP LEFT
picasso_rect src = {20, 80, bmp->width, bmp->height};

picasso_rect top_left_dst = {
    SPACING,
    SPACING,
    img_width,
    img_height
};

// TOP RIGHT
picasso_rect top_right_dst = {
    quad_width + SPACING,
    SPACING,
    img_width,
    img_height
};

// BOTTOM LEFT
picasso_rect bottom_left_dst = {
    SPACING,
    quad_height + SPACING,
    img_width,
    img_height
};

// BOTTOM RIGHT
picasso_rect bottom_right_dst = {
    quad_width + SPACING,
    quad_height + SPACING,
    img_width,
    img_height
};
    while (!canopy_window_should_close(win)) {
        if (canopy_should_render_frame()) {
            picasso_clear_backbuffer(bf);

            // Draw static center circle
            picasso_fill_circle(bf, WIDTH / 2, HEIGHT / 2, 5, PINK);

            // Draw left and right bitmaps
            picasso_blit(bf, bmp, src,  top_left_dst);
            picasso_blit(bf, grayscale, src, top_right_dst);
            picasso_blit(bf, red_chrominance, src,  bottom_left_dst);
            picasso_blit(bf, blue_chrominance,src,  bottom_right_dst);


            // Present the frame
            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
    }

    canopy_free_window(win);
    shutdown_log();
    return 0;
}
