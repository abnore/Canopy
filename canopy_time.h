#ifndef CANOPY_TIMER_H
#define CANOPY_TIMER_H

#include <stdint.h>

// Initialize the high-resolution timer (must be called once at startup)
void canopy_init_timer(void);

// Returns current time in seconds (high precision)
double canopy_get_time(void);

// Returns current time in nanoseconds
uint64_t canopy_get_time_ns(void);

// Set target FPS for frame limiting
void canopy_set_fps(int fps);

// Get the currently set target FPS
int canopy_get_fps(void);

// Returns whether enough time has passed to render a new frame
// Updates internal last_frame timestamp if true
int canopy_should_render_frame(void);

#endif // CANOPY_TIMER_H

