#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // isatty()

//----------------------------------------
// Types
//----------------------------------------

/**
 * @brief Defines the severity level of a log message.
 */
typedef enum {
    LOG_LEVEL_NONE  = 0,             /**< No logging */
    LOG_LEVEL_FATAL = 1 << 0,        /**< Fatal error, program cannot continue */
    LOG_LEVEL_ERROR = 1 << 1,        /**< Recoverable error, something went wrong */
    LOG_LEVEL_WARN  = 1 << 2,        /**< Warning, something unexpected but non-fatal */
    LOG_LEVEL_INFO  = 1 << 3,        /**< General informational messages */
    LOG_LEVEL_DEBUG = 1 << 4,        /**< Debugging information for developers */
    LOG_LEVEL_TRACE = 1 << 5,        /**< Fine-grained tracing details */

    LOG_LEVEL_ALL = LOG_LEVEL_FATAL |
                    LOG_LEVEL_ERROR |
                    LOG_LEVEL_WARN  |
                    LOG_LEVEL_INFO  |
                    LOG_LEVEL_DEBUG |
                    LOG_LEVEL_TRACE
} log_level;

/**
 * @brief Defines the destination type for log output.
 */
typedef enum {
    LOG_ERROR,   /**< Internal error in logger setup */
    LOG_FILE,    /**< Logging to a file */
    LOG_STDOUT   /**< Logging to standard output */
} log_type;

//----------------------------------------
// Runtime Log Configuration
//----------------------------------------

/**
 * @brief Enabled log levels as a bitmask. Toggle at runtime via API or env var.
 */
extern uint32_t log_levels_enabled;

//----------------------------------------
// Logger API
//----------------------------------------

/**
 * @brief Initializes the logging system.
 *
 * If `filepath` is `NULL`, logs are written to stdout.
 * If a valid path is given, logs are written to that file.
 *
 * @param filepath       Output file path, or NULL for stdout
 * @param enable_colors  Enable ANSI colors (auto-disabled for files)
 * @return `log_type` indicating output destination or failure
 */
log_type init_log(const char* filepath, bool enable_colors);

/**
 * @brief Gracefully shuts down the logger and releases resources.
 */
void shutdown_log(void);

/**
 * @brief Configures log level filtering using the `LOG_LEVELS` environment variable.
 *
 * Format supports:
 * - Comma-separated list of levels
 * - `+LEVEL` to enable, `-LEVEL` to disable
 * - Case-insensitive names (e.g., info, DEBUG)
 * - Special keywords: `ALL`, `NONE`
 *
 * Examples:
 * - `LOG_LEVELS=ALL,-TRACE`
 * - `LOG_LEVELS=+DEBUG,+INFO,-WARN`
 * - `LOG_LEVELS=none,+fatal`
 */
void configure_log_levels_from_env(void);

/**
 * @brief Enables or disables color output explicitly.
 *
 * Overrides automatic TTY detection.
 *
 * @param enabled True to force enable colors, false to disable
 */
void log_set_color_output(bool enabled);

/**
 * @brief Enable a specific log level at runtime.
 */
static inline void log_enable_level(log_level level) {
    log_levels_enabled |= level;
}

/**
 * @brief Disable a specific log level at runtime.
 */
static inline void log_disable_level(log_level level) {
    log_levels_enabled &= ~level;
}

/**
 * @brief Check whether a specific log level is enabled.
 */
static inline bool log_level_is_enabled(log_level level) {
    return (log_levels_enabled & level) != 0;
}

/**
 * @brief Logs a formatted message with full context (file, line, func).
 *
 * Normally used through macros like `DEBUG(...)`.
 */
void log_output_ext(log_level level, const char* file, int line, const char* func,
                    const char* msg, ...) __attribute__((format(printf, 5, 6)));

//----------------------------------------
// Logging Macros
//----------------------------------------

#define LOG(level, msg, ...) do { \
    if (log_level_is_enabled(level)) \
        log_output_ext(level, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__); \
} while(0)

#define FATAL(msg, ...)  LOG(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
#define ERROR(msg, ...)  LOG(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#define WARN(msg, ...)   LOG(LOG_LEVEL_WARN,  msg, ##__VA_ARGS__)
#define INFO(msg, ...)   LOG(LOG_LEVEL_INFO,  msg, ##__VA_ARGS__)
#define DEBUG(msg, ...)  LOG(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#define TRACE(msg, ...)  LOG(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)

//----------------------------------------
// Assertion Macros
//----------------------------------------

/**
 * @brief Called when an assertion fails.
 */
void report_assertion_failure(const char* expr, const char* msg, const char* file, int line);

/**
 * @brief Runtime assertion that logs failure (but does not abort).
 */
#define ASSERT(expr, msg) \
    if (!(expr)) { report_assertion_failure(#expr, msg, __FILE__, __LINE__); }

/**
 * @brief Compile-time assertion.
 */
#define BUILD_ASSERT(cond, msg) _Static_assert(cond, msg)

#endif // LOGGER_H
