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
// Log level toggles - 0 to disable
//----------------------------------------

/**
 * @brief Enable or disable WARN level logging.
 */
#define LOG_WARN_ENABLED   1

/**
 * @brief Enable or disable INFO level logging.
 */
#define LOG_INFO_ENABLED   1

/**
 * @brief Enable or disable DEBUG level logging.
 */
#define LOG_DEBUG_ENABLED  1

/**
 * @brief Enable or disable TRACE level logging.
 */
#define LOG_TRACE_ENABLED  1

//----------------------------------------
// Types
//----------------------------------------

/**
 * @brief Defines the severity level of a log message.
 */
typedef enum {
    LOG_LEVEL_FATAL = 0,  /**< Fatal error, program cannot continue */
    LOG_LEVEL_ERROR,      /**< Recoverable error, something went wrong */
    LOG_LEVEL_WARN,       /**< Warning, something unexpected but non-fatal */
    LOG_LEVEL_INFO,       /**< General informational messages */
    LOG_LEVEL_DEBUG,      /**< Debugging information for developers */
    LOG_LEVEL_TRACE       /**< Fine-grained tracing details */
} log_level;

/**
 * @brief Defines the destination for log output.
 */
typedef enum {
    LOG_ERROR,   /**< Internal error in logger setup */
    LOG_FILE,    /**< Logging to a file */
    LOG_STDOUT   /**< Logging to standard output */
} log_type;

//----------------------------------------
// Logger API
//----------------------------------------

/**
 * @brief Initializes the logger.
 *
 * If `filepath` is NULL, logs go to stdout.
 * Automatically enables ANSI colors if output is a TTY.
 *
 * @param filepath File to write logs to, or NULL to use stdout
 * @param enable_colors Whether to enable ANSI color output
 * @return LOG_FILE if logging to file, LOG_STDOUT if stdout fallback, LOG_ERROR on failure
 */
log_type init_log(const char* filepath, bool enable_colors);

/**
 * @brief Shuts down the logger and closes file handles if necessary.
 */
void shutdown_log(void);

/**
 * @brief Manually enable or disable colored output.
 *
 * This overrides automatic detection of TTY/file.
 *
 * @param enabled True to enable color output, false to disable
 */
void log_set_color_output(bool enabled);

/**
 * @brief Logs a formatted message with full context (level, file, line, func).
 *
 * This function is usually wrapped with the provided macros.
 *
 * @param level Log severity level
 * @param file Source file name
 * @param line Line number in file
 * @param func Function name
 * @param msg Format string
 * @param ... Format arguments
 */
void log_output_ext(log_level level, const char* file, int line, const char* func, const char* msg, ...) __attribute__((format(printf, 5, 6)));

//----------------------------------------
// Logging macros
//----------------------------------------

/**
 * @brief Base logging macro with full context.
 */
#define LOG(level, msg, ...) \
    log_output_ext(level, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__)

/**
 * @brief Logs a fatal error and terminates the program.
 */
#define FATAL(msg, ...)  LOG(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)

/**
 * @brief Logs a serious error that should be addressed.
 */
#ifndef ERROR
#define ERROR(msg, ...)  LOG(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#endif

/**
 * @brief Logs a warning. The program can continue.
 */
#if LOG_WARN_ENABLED
#define WARN(msg, ...)   LOG(LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#else
#define WARN(msg, ...)
#endif

/**
 * @brief Logs general information about program execution.
 */
#if LOG_INFO_ENABLED
#define INFO(msg, ...)   LOG(LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#else
#define INFO(msg, ...)
#endif

/**
 * @brief Logs detailed information helpful for debugging.
 */
#if LOG_DEBUG_ENABLED
#define DEBUG(msg, ...)  LOG(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define DEBUG(msg, ...)
#endif

/**
 * @brief Logs fine-grained tracing details for deep inspection.
 */
#if LOG_TRACE_ENABLED
#define TRACE(msg, ...)  LOG(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define TRACE(msg, ...)
#endif

//----------------------------------------
// Optional assertion macros
//----------------------------------------

/**
 * @brief Reports a failed assertion with detailed context.
 *
 * This function is called by the ASSERT macro.
 *
 * @param expression The failed expression as a string.
 * @param message    Custom error message.
 * @param file       Source file.
 * @param line       Line number.
 */
void report_assertion_failure(const char* expression, const char* message, const char* file, int line);

/**
 * @brief Custom runtime assertion that logs on failure instead of aborting.
 *
 * Use this to check conditions during runtime without crashing the program.
 *
 * @param expr The expression to test.
 * @param msg  The message to log if the assertion fails.
 */
#define ASSERT(expr, msg) \
    if (!(expr)) { report_assertion_failure(#expr, msg, __FILE__, __LINE__); }

/**
 * @brief Compile-time assertion for validating constants, struct layouts, etc.
 *
 * This macro wraps `_Static_assert`, which performs the check at compile time.
 *
 * Use it to catch errors early, like mismatched struct sizes or invalid enums.
 *
 * @param cond The condition to assert (must be evaluable at compile time).
 * @param msg  The message to display if the assertion fails.
 */
#define BUILD_ASSERT(cond, msg) _Static_assert(cond, msg)


#endif // LOGGER_H
