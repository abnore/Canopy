#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "picasso.h"
#include "canopy.h"

#define WIDTH 800
#define HEIGHT 600

int main(void) {
    init_log(LOG_DEFAULT);

    canopy_init_timer();
    canopy_window *window = canopy_create_window("Font Test", WIDTH, HEIGHT, CANOPY_WINDOW_STYLE_DEFAULT);
    picasso_backbuffer *bf = picasso_create_backbuffer(WIDTH, HEIGHT);

    // Load font
    const char *font_path = "/Users/andreas/personalC/Picasso/test/fonts/Alegreya,Libre_Baskerville/Libre_Baskerville/LibreBaskerville-Regular.ttf";
    picasso_reader *reader = picasso_read_entire_file(font_path);
    if (!reader) FATAL("Failed to read font file");

    stbtt_fontinfo font;
    if (!stbtt_InitFont(&font, reader->ptr, stbtt_GetFontOffsetForIndex(reader->ptr, 0)))
        FATAL("Failed to init font");

    float size = 64.0f;
    float scale = stbtt_ScaleForPixelHeight(&font, size);

    // Get vertical baseline
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    int baseline = (int)(ascent * scale);

    const char *text = "Hello M B P X A N a b c";
    color c = PURPLE;
    int x = 10;  // starting x pos

    while (!canopy_window_should_close(window)) {
        if (canopy_should_render_frame()) {
            picasso_clear_backbuffer(bf);

            x = 10;  // reset per frame
            for (const char *p = text; *p; ++p) {
                int w, h, xoff, yoff;
                uint8_t *bitmap = stbtt_GetCodepointBitmap(
                    &font, 0, scale, *p, &w, &h, &xoff, &yoff);

                draw_bitmap_to_backbuffer(bf, bitmap, w, h, x + xoff, 300 - baseline + yoff, c);
                stbtt_FreeBitmap(bitmap, NULL);

                // advance x position
                int ax;
                stbtt_GetCodepointHMetrics(&font, *p, &ax, 0);
                x += (int)(ax * scale);

                // optional kerning
                if (*(p + 1)) {
                    x += (int)(stbtt_GetCodepointKernAdvance(&font, *p, *(p + 1)) * scale);
                }
            }

            canopy_swap_backbuffer(window, (framebuffer *)bf);
            canopy_present_buffer(window);
        }
    }

    picasso_reader_free(reader);
    shutdown_log();
    return 0;
}
