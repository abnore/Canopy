#ifndef LOGGER_H
#define LOGGER_H

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

typedef enum {
	LOG_LEVEL_FATAL,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_TRACE
} log_level;

static FILE* log_output_stream = NULL;

int initialize_logging();
void shutdown_logging();

void log_output(log_level level, const char *msg, ...) __attribute__((format(printf, 2, 3)));

#define FATAL(msg, ...) log_output(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__);

#ifndef ERROR
#define ERROR(msg, ...) log_output(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define WARN(msg, ...) log_output(LOG_LEVEL_WARN, msg, ##__VA_ARGS__);
#else
#define WARN(msg, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define INFO(msg, ...) log_output(LOG_LEVEL_INFO, msg, ##__VA_ARGS__);
#else
#define INFO(msg, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define DEBUG(msg, ...) log_output(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__);
#else
#define DEBUG(msg, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define TRACE(msg, ...) log_output(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__);
#else
#define TRACE(msg, ...)
#endif

#endif // LOGGER_H
