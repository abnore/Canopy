#include <math.h>

#include "canopy.h"
#include "picasso.h"
#include <blackbox.h>

#define WIDTH 800
#define HEIGHT 600

#define RADIUS 5
#define CIRCLE_COLOR CYAN
#define GRID_COUNT 10
#define GRID_PAD 0.5/GRID_COUNT
#define GRID_SIZE ((GRID_COUNT - 1)*GRID_PAD)
#define Z_START 0.25

void render3d(picasso_backbuffer *bf, float *angle, double dt);

int main(void)
{
    init_log(LOG_DEFAULT);
    set_fps(60);

    Window *win = create_window("3D test",
                        WIDTH, HEIGHT,
                        CANOPY_WINDOW_STYLE_TITLED |
                        CANOPY_WINDOW_STYLE_CLOSABLE);

    picasso_backbuffer *bf = picasso_create_backbuffer(win);

    float angle = 0;
    while(!window_should_close(win))
    {
        pump_messages();
        // Set up and input
        // ----------------
        if(should_render_frame())
        {
            picasso_clear_backbuffer(bf);

            render3d(bf, &angle, get_delta_time());

            swap_backbuffer(win, (framebuffer*)bf);
            present_buffer(win);
        }
    }

    free_window(win);
    shutdown_log();
    return 0;
}

// In order to work conceptually with a 3d grid it makes sense to normalize the
// coords Therefore we can say that z = 0 is the screen, and screen positive
// (or negative) is visable
void render3d(picasso_backbuffer *bf, float *angle, double dt)
{
    *angle += 0.25*M_PI*dt;

    for( int ix = 0; ix < GRID_COUNT; ++ix){
        for( int iy = 0; iy < GRID_COUNT; ++iy){
            for( int iz = 0; iz < GRID_COUNT; ++iz){
                float x = ix*GRID_PAD - GRID_SIZE/2; //-- these are now from -1 to 1
                float y = iy*GRID_PAD - GRID_SIZE/2;
                float z = Z_START + iz*GRID_PAD;

                float cx = 0.0;
                float cz = Z_START + GRID_SIZE/2;

                float dx = x - cx; // finding the dist between them
                float dz = z - cz;

                float a = atan2f(dz, dx); // angle between them
                float m = sqrtf(dx*dx + dz*dz); //magnitude

                dx = cosf(a + *angle)*m; // turn into cosine and sine
                dz = sinf(a + *angle)*m;

                x = dx + cx; // turn them back into coordinated on the "screen"
                z = dz + cz;

                x /= z;
                y /= z;

                color c = {
                    .r = ix*255/GRID_COUNT,
                    .g = iy*255/GRID_COUNT,
                    .b = iz*255/GRID_COUNT,
                    .a = 255,
                };
                // (-1, (..), 1 plus one is 0-2 and then divided by 2 is 0-1 *times width = 0-width! Normalized!
                float depth_factor = 1.0f - (z - Z_START) / GRID_SIZE;
                depth_factor = PICASSO_CLAMP(depth_factor, 0.0f, 1.0f);

                // Reduce variation — blend toward a neutral midpoint like 0.75
                depth_factor = 0.75f + 0.25f * depth_factor;
                depth_factor = 1.0f / (1.0f + 2.0f * (z - Z_START));
                depth_factor = PICASSO_CLAMP(depth_factor, 0.75f, 1.0f); // keep it gentle
                z = fmaxf(z, Z_START + 0.05f); // avoid z getting too small
                float radius = RADIUS * depth_factor;
                picasso_fill_circle_aa(bf, (x + 1) / 2 * WIDTH, (y + 1) / 2 * HEIGHT, radius, c);
            }
        }
    }
}
