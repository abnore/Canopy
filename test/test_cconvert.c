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

#define WIDTH 1200
#define HEIGHT 600

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
    picasso_image *bmp = picasso_load_bmp("assets/sample.bmp");
    if (!bmp) {
        ERROR("Could not load sample.bmp");
        return 1;
    }

    picasso_image *converted = picasso_alloc_image(bmp->width, bmp->height, 4);
    if (!converted) {
        ERROR("Could not allocate output image");
        return 1;
    }

#define SHOW_COMPONENT_Y    0
#define SHOW_COMPONENT_CB   0
#define SHOW_COMPONENT_CR   1
#define SHOW_RECONSTRUCTED  0  // Set to 1 to show full RGB after round-trip

    foreach_pixel(bmp, {
            float r = pixels[0];
            float g = pixels[1];
            float b = pixels[2];

            // RGB → YCbCr
            float Y  = (0.229f * r) + (0.587f * g) + (0.114f * b);
            float Cb = 128.0f - (0.168736f * r) - (0.331264f * g) + (0.5f * b);
            float Cr = 128.0f + (0.5f * r) - (0.418688f * g) - (0.081312f * b);

            #if SHOW_RECONSTRUCTED
            // YCbCr → RGB (round trip)
            float rf = Y + 1.402f * (Cr - 128);
            float gf = Y - 0.344136f * (Cb - 128) - 0.714136f * (Cr - 128);
            float bf = Y + 1.772f * (Cb - 128);

            pixels[0] = (uint8_t)PICASSO_CLAMP(rf, 0, 255);
            pixels[1] = (uint8_t)PICASSO_CLAMP(gf, 0, 255);
            pixels[2] = (uint8_t)PICASSO_CLAMP(bf, 0, 255);

            #elif SHOW_COMPONENT_Y
            uint8_t v = (uint8_t)PICASSO_CLAMP(Y, 0, 255);
            pixels[0] = pixels[1] = pixels[2] = v;

            #elif SHOW_COMPONENT_CB
            uint8_t v = (uint8_t)PICASSO_CLAMP(Cb, 0, 255);
            pixels[0] = 0;
            pixels[1] = 0;
            pixels[2] = v;

            #elif SHOW_COMPONENT_CR
            uint8_t v = (uint8_t)PICASSO_CLAMP(Cr, 0, 255);
            pixels[0] = v;
            pixels[1] = Y;
            pixels[2] = Cb;
            #endif

            // Preserve alpha if present
            if (bmp->channels == 4)
                pixels[3] = 255;
    });

    canopy_init_timer();
    canopy_set_fps(24);

    while (!canopy_window_should_close(win)) {
        if (canopy_should_render_frame()) {
            picasso_clear_backbuffer(bf);
            picasso_blit_bitmap(bf, bmp->pixels, bmp->width, bmp->height, 0, 0);
            picasso_blit_bitmap(bf, converted->pixels, converted->width, converted->height, bmp->width + 10, 0);

            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
    }

    canopy_free_window(win);
    shutdown_log();
    return 0;
}
