#include <math.h>

#include "canopy.h"
#include "picasso.h"

//#define FACTOR 100
//#define WIDTH (16*FACTOR)
//#define HEIGHT (9*FACTOR)

#define WIDTH 800
#define HEIGHT 600

#define RADIUS 5
#define CIRCLE_COLOR CYAN
#define GRID_COUNT 10
#define GRID_PAD 0.5/GRID_COUNT
#define GRID_SIZE ((GRID_COUNT -1)*GRID_PAD)
#define Z_START 0.25

void render3d(picasso_backbuffer *bf, float *angle, double dt);

int main(void)
{
    init_log(NO_LOG, LOG_COLORS, STDERR_TO_LOG);
    canopy_init_timer();
    canopy_set_fps(60);

    canopy_window *win = canopy_create_window("3D test",
                        WIDTH, HEIGHT,
                        CANOPY_WINDOW_STYLE_TITLED |
                        CANOPY_WINDOW_STYLE_CLOSABLE);

    picasso_backbuffer *bf = picasso_create_backbuffer(WIDTH, HEIGHT);

    float angle = 0;
    while(!canopy_window_should_close(win))
    {
        // Set up and input
        // ----------------
        if(canopy_should_render_frame())
        {
            picasso_clear_backbuffer(bf);

            render3d(bf, &angle, canopy_get_delta_time());

            canopy_swap_backbuffer(win, (framebuffer*)bf);
            canopy_present_buffer(win);
        }
    }

    canopy_free_window(win);
    shutdown_log();
    return 0;
}

// In order to work conceptually with a 3d grid it makes sense to normalize the coords
// Therefore we can say that z = 0 is the screen, and screen positive (or negative) is visable
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
                // (-1, (..), 1 plus one is 0-2 and then divided by 2 is 0-1 *times width = 0-width! Normalized!

                color c = {
                    .r = ix*255/GRID_COUNT,
                    .g = iy*255/GRID_COUNT,
                    .b = iz*255/GRID_COUNT,
                    .a = 255,
                };
                picasso_fill_circle_aa(bf,(x+1)/2*WIDTH,(y+1)/2*HEIGHT, RADIUS, c);
            }
        }
    }
}
