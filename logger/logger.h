#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------
// Log level toggles
//----------------------------------------
#define LOG_WARN_ENABLED   1
#define LOG_INFO_ENABLED   1
#define LOG_DEBUG_ENABLED  1
#define LOG_TRACE_ENABLED  1

#if RELEASE == 1
    #undef  LOG_DEBUG_ENABLED
    #define LOG_DEBUG_ENABLED 0
    #undef  LOG_TRACE_ENABLED
    #define LOG_TRACE_ENABLED 0
#endif

//----------------------------------------
// Types
//----------------------------------------

typedef enum {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} log_level;

//----------------------------------------
// Logger API
//----------------------------------------

/**
 * Initializes the logger.
 * If filepath is NULL, logs go to stdout.
 * ANSI color will be automatically enabled for TTY, disabled for files.
 * Returns: 0 = error, 1 = file ok, 2 = stdout fallback
 */
int init_log(const char* filepath, bool enable_colors);

/**
 * Shuts down the logger (closes file, if any).
 */
void shutdown_log();

/**
 * Overrides automatic color logic.
 */
void log_set_color_output(bool enabled);

/**
 * Logs a message with full context.
 */
void log_output_ext(log_level level, const char* file, int line, const char* func, const char* msg, ...) __attribute__((format(printf, 5, 6)));

//----------------------------------------
// Logging macros
//----------------------------------------

#define LOG(level, msg, ...) \
    log_output_ext(level, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)

#define FATAL(msg, ...)  LOG(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)

#ifndef ERROR
#define ERROR(msg, ...)  LOG(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#endif

#if LOG_WARN_ENABLED
#define WARN(msg, ...)   LOG(LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#else
#define WARN(msg, ...)
#endif

#if LOG_INFO_ENABLED
#define INFO(msg, ...)   LOG(LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#else
#define INFO(msg, ...)
#endif

#if LOG_DEBUG_ENABLED
#define DEBUG(msg, ...)  LOG(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define DEBUG(msg, ...)
#endif

#if LOG_TRACE_ENABLED
#define TRACE(msg, ...)  LOG(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define TRACE(msg, ...)
#endif

//----------------------------------------
// Optional assertion macro
//----------------------------------------

void report_assertion_failure(const char* expression, const char* message, const char* file, int line);

#define ASSERT(expr, msg) \
    if (!(expr)) { report_assertion_failure(#expr, msg, __FILE__, __LINE__); }

#endif // LOGGER_H

