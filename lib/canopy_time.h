#ifndef CANOPY_TIMER_H
#define CANOPY_TIMER_H

#include <stdint.h>

/*
 * Initializes the timer system.
 * Must be called before any other time-related functions.
 */
void canopy_init_timer(void);

/*
 * Returns current time in seconds, as a double,
 * and get time in nanoseconds
 */
double canopy_get_time(void);
uint64_t canopy_get_time_ns(void);

/*
 * Gets delta time between last rendered frame and the current one.
 * Used for smooth animations and motion consistency
 */
double canopy_get_delta_time(void);

/*
 * Set the target FPS for rendereing. Defaults to 60 fps, and clamped at 1
 */
void canopy_set_fps(int fps);
int canopy_get_fps(void);

/*
 * Asks is rendereing should occur, checks if enough time has passed to render next
 * frame. 1 if true, 0 is false
 */
int canopy_should_render_frame(void);

#endif // CANOPY_TIMER_H
