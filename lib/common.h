#ifndef COMMON_H
#define COMMON_H

#ifndef CUSTOM_ALLOCATOR
    #include <stdlib.h>   // Fallback to standard allocator if custom not defined
#endif

#include <stdio.h>
#include <string.h>
#include <blackbox.h>
#include <stdint.h>
#include <stdbool.h>

#include "input.h"        // Input keys and mouse buttons
#include "canopy_time.h"  // Timing utilities

//------------------------------------------------------------------------------
// Memory Allocation
//------------------------------------------------------------------------------
/* Custom allocators or wrappers if not defined */
void *canopy_calloc(size_t count, size_t size);
void canopy_free(void *ptr);
void *canopy_malloc(size_t size);
void *canopy_realloc(void *ptr, size_t size);


/*
 * Convert a keyboard key to a human-readable string.
 */
const char* canopy_key_to_string(keys key);

#endif // COMMON_H
