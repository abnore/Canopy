#include <math.h>
#include "canopy.h"
#include "picasso.h"

#define WIDTH 800
#define HEIGHT 600
#define GRID_SPACING 0.2f
#define GRID_COUNT 21
#define SCALE 250.0f

typedef struct {
    float x, y, z;
} vec3;

vec3 rotate_z(vec3 p, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    return (vec3){
        .x = p.x * c - p.y * s,
        .y = p.x * s + p.y * c,
        .z = p.z
    };
}

vec3 rotate_x(vec3 p, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    return (vec3){
        .x = p.x,
        .y = p.y * c - p.z * s,
        .z = p.y * s + p.z * c
    };
}

void project_and_draw_line(picasso_backbuffer *bf, vec3 a, vec3 b, color c) {
    float d = 2.5f; // camera "distance" to scene, push everything back

    // skip if behind the camera
    if ((a.z + d) <= 0.01f || (b.z + d) <= 0.01f)
        return;

    float sx0 = (a.x / (a.z + d)) * SCALE + WIDTH / 2;
    float sy0 = (-a.y / (a.z + d)) * SCALE + HEIGHT / 2;
    float sx1 = (b.x / (b.z + d)) * SCALE + WIDTH / 2;
    float sy1 = (-b.y / (b.z + d)) * SCALE + HEIGHT / 2;

    picasso_draw_line(bf, (int)sx0, (int)sy0, (int)sx1, (int)sy1, c);
}

void draw_rotated_grid(picasso_backbuffer *bf) {
    float half = GRID_SPACING * (GRID_COUNT - 1) / 2.0f;
    color grid_color = (color){0x0, 0xff, 0xff, 0xff};

    float rot_x = 60.0f * M_PI / 180.0f;
    float rot_z = 80.0f * M_PI / 180.0f;

    for (int i = 0; i < GRID_COUNT; ++i) {
        float offset = -half + i * GRID_SPACING;

        // Horizontal lines (varying y)
        vec3 a = { -half, offset, 0 };
        vec3 b = {  half, offset, 0 };
        a = rotate_x(rotate_z(a, rot_z), rot_x);
        b = rotate_x(rotate_z(b, rot_z), rot_x);
        project_and_draw_line(bf, a, b, grid_color);

        // Vertical lines (varying x)
        vec3 c = { offset, -half, 0 };
        vec3 d = { offset,  half, 0 };
        c = rotate_x(rotate_z(c, rot_z), rot_x);
        d = rotate_x(rotate_z(d, rot_z), rot_x);
        project_and_draw_line(bf, c, d, grid_color);
    }
}

int main(void) {
    canopy_init_timer();
    canopy_window *win = canopy_create_window("Rotated Grid", WIDTH, HEIGHT, CANOPY_WINDOW_STYLE_DEFAULT);
    picasso_backbuffer *bf = picasso_create_backbuffer(WIDTH, HEIGHT);

    while (!canopy_window_should_close(win)) {
        if (canopy_should_render_frame()) {
            picasso_clear_backbuffer(bf);
            draw_rotated_grid(bf);
            canopy_swap_backbuffer(win, (framebuffer *)bf);
            canopy_present_buffer(win);
        }
    }

    canopy_free_window(win);
    return 0;
}
