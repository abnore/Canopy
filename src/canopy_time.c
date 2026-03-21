#include "canopy.h"

#include <blackbox.h>
#include <mach/mach_time.h>
#include <math.h>

#define DEFAULT_FPS 60
#define MAX_FRAME_DELTA 0.25   // Clamp huge pauses (debugger, app stall, etc.)

static struct {
    mach_timebase_info_data_t timebase;
    double to_seconds;

    double target_frame_time;   // Desired seconds per frame
    double last_tick_time;      // Last time should_render_frame() checked
    double last_frame_time;     // Last time a frame was actually accepted
    double delta_time;          // Seconds between accepted frames
    double accumulator;         // Leftover elapsed time
    int initialized;
} canopy_timer;

void init_timer(void)
{
    kern_return_t result = mach_timebase_info(&canopy_timer.timebase);
    if (result != KERN_SUCCESS) {
        FATAL("Failed to initialize mach timebase");
        return;
    }

    canopy_timer.to_seconds =
        ((double)canopy_timer.timebase.numer /
         (double)canopy_timer.timebase.denom) / 1e9;

    canopy_timer.target_frame_time = 1.0 / DEFAULT_FPS;

    double now = (double)mach_absolute_time() * canopy_timer.to_seconds;

    canopy_timer.last_tick_time = now;
    canopy_timer.last_frame_time = now;
    canopy_timer.delta_time = 1.0 / DEFAULT_FPS;
    canopy_timer.accumulator = 0.0;
    canopy_timer.initialized = 1;

    INFO("Timer initialized");
}

double get_time(void)
{
    return (double)mach_absolute_time() * canopy_timer.to_seconds;
}

uint64_t get_time_ns(void)
{
    uint64_t ticks = mach_absolute_time();
    return ticks * canopy_timer.timebase.numer / canopy_timer.timebase.denom;
}

/* Users can normalize themselves, this is raw time since last frame
 * E.g. double dt = get_delta_time()*60; if fps is 60
 * This is the same as '/ (1/60)' which is normalization */
double get_delta_time(void)
{
    return canopy_timer.delta_time;
}

void set_fps(int fps)
{
    if (fps < 1) {
        WARN("FPS below 1 specified, clamping to 1");
        fps = 1;
    }

    canopy_timer.target_frame_time = 1.0 / (double)fps;
    canopy_timer.accumulator = 0.0;
}

int get_fps(void)
{
    return (int)(1.0 / canopy_timer.target_frame_time);
}

int should_render_frame(void)
{
    if (!canopy_timer.initialized) {
        WARN("Timer used before init_timer()");
        return 0;
    }

    double now = get_time();
    double elapsed = now - canopy_timer.last_tick_time;
    canopy_timer.last_tick_time = now;

    if (elapsed < 0.0)
        elapsed = 0.0;
    if (elapsed > MAX_FRAME_DELTA)
        elapsed = MAX_FRAME_DELTA;

    canopy_timer.accumulator += elapsed;

    if (canopy_timer.accumulator < canopy_timer.target_frame_time)
        return 0;

    canopy_timer.delta_time = now - canopy_timer.last_frame_time;
    if (canopy_timer.delta_time > MAX_FRAME_DELTA)
        canopy_timer.delta_time = MAX_FRAME_DELTA;

    canopy_timer.last_frame_time = now;

    /* Keep only the leftover fractional time. This gives stable pacing without
     * building up a backlog of "missed frames". */
    canopy_timer.accumulator = fmod(canopy_timer.accumulator,
                                    canopy_timer.target_frame_time);

    return 1;
}
