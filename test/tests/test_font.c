#include "canopy.h"

#include <blackbox.h>
#include <string.h>
#include <unistd.h>

#define WIDTH 800
#define HEIGHT 600

int main(void) {
    init_log(LOG_DEFAULT);

    Window *window = create_window("Font Test", WIDTH, HEIGHT,
                                   CANOPY_WINDOW_STYLE_DEFAULT);
    Framebuffer *fb = get_framebuffer(window);
    set_fps(5);
    const char *font_path = ("./fonts/LibreBaskerville-Bold.ttf");
    //const char *font_path = ("LibreBaskerville-Bold");
    //const char *font_path = ("Helvetica");
    //const char *font_path = (NULL);

    FontBounds b;
    Font *font = load_font(font_path);
    Renderer *renderer = create_renderer(fb);

    Color color = {0xdc/255.f, 0xd7/255.f, 0xba/255.f, 1.0f};

    char counter = '!';
    char text[2] = {0};

    float size = 84.0f;
    int count = 0;
    int max_per_row = 20;
    float row_height = 100.0f;

    float x_start = 40.0f;
    float y_start = 20.0f;

    float x_cursor = x_start;
    float y_cursor = y_start;

    while (!window_should_close(window))
    {
        pump_messages();

        if (should_render_frame())
        {
            text[0] = counter;
            text[1] = '\0';

            b = draw_text(renderer, font, text, x_cursor, y_cursor, size, color);

            x_cursor += (float)b.width;
            count++;

            if (count >= max_per_row) {
                count = 0;
                x_cursor = x_start;
                y_cursor += row_height;
            }

            if (counter > '~') {
                sleep(3);
                count = 0;
                counter = '0';
                x_cursor = 40.f;
                y_cursor = 20.f;
                memset(fb->pixels, 0, fb->buffer_size);
            }
            counter++;

            present_buffer(window);
        }
}

    destroy_renderer(renderer);
    destroy_font(font);

    free_window(window);
    shutdown_log();
    return 0;
}
