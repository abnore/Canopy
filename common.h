#ifndef COMMON_H
#define COMMON_H

#ifdef NO_CUSTOM_ALLOCATOR
    #include <stdlib.h>
#endif

#define LOG_WARN_ENABLED 	1
#define LOG_INFO_ENABLED 	1
#define LOG_TRACE_ENABLED 	1
#define LOG_DEBUG_ENABLED 	0

enum LOG_LEVEL {
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_INFO,
	LOG_LEVEL_TRACE,
	LOG_LEVEL_DEBUG,
};

typedef enum LOG_LEVEL log_level;

void logger(log_level level, const char *msg, ...);

#ifndef ERROR
#define ERROR(msg, ...) logger(LOG_LEVEL_ERROR, "%s:%d "msg,__FILE__,__LINE__, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED
#define WARN(msg, ...) logger(LOG_LEVEL_WARNING, "%s:%d "msg, __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define WARN(msg, ...)
#endif

#if LOG_INFO_ENABLED
#define INFO(msg, ...) logger(LOG_LEVEL_INFO, msg, ##__VA_ARGS__);
#else
#define INFO(msg, ...)
#endif

#if LOG_TRACE_ENABLED
#define TRACE(msg, ...) logger(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__);
#else
#define TRACE(msg, ...)
#endif

#if LOG_DEBUG_ENABLED
#define DEBUG(msg, ...) logger(LOG_LEVEL_DEBUG,"%s:%d "msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define MALLOC(size)	debug_malloc(size, __FILE__, __LINE__)
#define malloc(size)	MALLOC(size)
#define FREE(ptr)	debug_free(ptr, #ptr, __FILE__, __LINE__)
#define free(ptr)	FREE(ptr)
void debug_free(void *ptr, const char* name, const char *file, const int line);
void *debug_malloc(size_t size, const char *file, const int line);
#else
#define DEBUG(msg, ...)
#endif



void logger(log_level level, const char *msg, ...)
{
	const char *level_strings[5] = {"[ERROR]: ",
									"[WARN]: ",
									"[INFO]: ",
									"[ ]: ",
									"[ !!! ]: " };
	char buffer_message[4096] = {0};

	va_list arg_ptr;

	va_start(arg_ptr, msg);
	vsnprintf(buffer_message, 4096, msg, arg_ptr);
	va_end(arg_ptr);

	char out_message[4096] = {0};
	snprintf(out_message, 4096, "%s%s", level_strings[level], buffer_message);

	printf("%s\n", out_message);
}

#if LOG_DEBUG_ENABLED
void debug_free(void *ptr, const char* name, const char *file, const int line)
{
	if (!ptr) {
		return; // Unngår å free NULL-pointers
	}
	printf("[ FREE ]: %s:%d memory freed for variable %s\n", file, line, name);
	#undef free
	free(ptr);
	#define free(ptr) FREE(ptr)
}

void *debug_malloc(size_t size, const char *file, const int line)
{
	static size_t total_stored = 0;
	total_stored += size;

	#undef malloc
	void *ptr = malloc(size);
	#define malloc(size) MALLOC(size)

	if (!ptr) {
		return NULL; // vil slå ut som error av kaller
	}

	printf("[ MALLOC ] :%s:%d %zu bytes allocated. Total allocated %zu bytes\n",
		file, line, size, total_stored);
	return ptr;
}
#endif



#endif // COMMON_H
