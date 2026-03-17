#include "canopy.h"

/* Custom allocators can come here, wrappers over stdlib for now.*/
void *canopy_calloc(size_t count, size_t size)
{
    return calloc(count, size);
}
void canopy_free(void *ptr)
{
    free(ptr);
}
void *canopy_malloc(size_t size)
{
    return malloc(size);
}
void *canopy_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}
