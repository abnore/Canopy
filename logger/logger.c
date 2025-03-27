#include "logger.h"


static FILE* log_output_stream = NULL;
static bool log_colors_enabled = true;
static bool log_color_auto = true;

static const char* level_strings[] = {
    "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"
};

static const char* level_colors[] = {
    "\033[1;31m", // FATAL - bright red
    "\033[0;31m", // ERROR - red
    "\033[0;33m", // WARN  - yellow
    "\033[0;32m", // INFO  - green
    "\033[0;36m", // DEBUG - cyan
    "\033[0;90m"  // TRACE - gray
};

static const char* color_reset = "\033[0m";

log_type init_log(const char* filepath, bool enable_colors)
{
    log_color_auto = true;
    log_output_stream = NULL;

    if (!filepath) {
        log_output_stream = stdout;
        log_colors_enabled = enable_colors && isatty(fileno(stdout));
        return 2;
    }

    log_output_stream = fopen(filepath, "w");
    if (!log_output_stream) {
        fprintf(stderr, "[LOGGER ERROR] Failed to open log file: %s\n", filepath);
        return 0;
    }

    log_colors_enabled = false; // force off for file logs
    return 1;
}

void log_set_color_output(bool enabled)
{
    log_color_auto = false;
    log_colors_enabled = enabled;
}

void shutdown_log()
{
    if (log_output_stream) {
        fflush(log_output_stream);

        if (log_output_stream != stdout &&
            log_output_stream != stderr) {
            fclose(log_output_stream);
        }

        log_output_stream = NULL;
    }
}

void log_output_ext(log_level level, const char* file, int line, const char* func, const char* msg, ...)
{
    if (!log_output_stream) {
        log_output_stream = stdout;
    }

    // Wall-clock time
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char timestamp[16];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", t);

    // Format the user message
    char user_msg[32000];
    va_list args;
    va_start(args, msg);
    vsnprintf(user_msg, sizeof(user_msg), msg, args);
    va_end(args);

    // Final output
    FILE* out = log_output_stream;
    fprintf(out,
        "%s[%s] [%s] %s:%d (%s): %s%s\n",
        log_colors_enabled ? level_colors[level] : "",
        timestamp,
        level_strings[level],
        file,
        line,
        func,
        user_msg,
        log_colors_enabled ? color_reset : "");

    fflush(out);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, int line)
{
    log_output_ext(LOG_LEVEL_FATAL, file, line, "ASSERT", "Assertion failed: %s — %s", expression, message);
}
