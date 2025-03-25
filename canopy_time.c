#include "canopy_time.h"
#include <mach/mach_time.h>
#include <stdio.h>

static struct {
    mach_timebase_info_data_t timebase;
    double to_seconds;

    double target_frame_time;     // e.g., 1.0 / 60.0
    double last_frame_time;       // last time a frame was rendered
} canopy_timer;

void canopy_init_timer(void)
{
    mach_timebase_info(&canopy_timer.timebase);
    canopy_timer.to_seconds = (double)canopy_timer.timebase.numer /
                              (double)canopy_timer.timebase.denom / 1e9;

    canopy_timer.target_frame_time = 1.0 / 60.0; // default to 60 FPS
    canopy_timer.last_frame_time = canopy_get_time(); // initialize timestamp
}

double canopy_get_time(void)
{
    return (double)mach_absolute_time() * canopy_timer.to_seconds;
}

uint64_t canopy_get_time_ns(void)
{
    uint64_t ticks = mach_absolute_time();
    return ticks * canopy_timer.timebase.numer / canopy_timer.timebase.denom;
}

void canopy_set_fps(int fps)
{
    if (fps < 1) fps = 1;
    canopy_timer.target_frame_time = 1.0 / (double)fps;
}

int canopy_get_fps(void)
{
    return (int)(1.0 / canopy_timer.target_frame_time);
}

int canopy_should_render_frame(void)
{
    double now = canopy_get_time();
    double elapsed = now - canopy_timer.last_frame_time;

    if (elapsed >= canopy_timer.target_frame_time) {
        canopy_timer.last_frame_time = now;
        return 1; // Yes, go ahead and render
    }

    return 0; // No, skip this frame
}

