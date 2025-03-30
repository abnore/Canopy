#include <stdlib.h>     // for getenv, strdup
#include <string.h>     // for strtok, strcasecmp
#include <stdio.h>      // for fprintf, fopen
#include <stdarg.h>     // for va_list
#include <stdbool.h>    // for bool
#include <time.h>       // for time, localtime
#include <unistd.h>     // for isatty, fileno

#include "logger.h"

FILE* log_output_stream = NULL;
uint32_t log_levels_enabled = LOG_LEVEL_ALL;

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

void configure_log_levels_from_env(void)
{
    const char* env = getenv("LOG_LEVELS");
    if (!env) return;

    char* input = strdup(env);
    char* token = strtok(input, ",");

    while (token) {
        while (*token == ' ') token++; // trim leading space

        bool disable = false;
        if (*token == '-') {
            disable = true;
            token++;
        } else if (*token == '+') {
            token++;
        }

        if (strcasecmp(token, "ALL") == 0) {
            log_levels_enabled = LOG_LEVEL_ALL;
        } else if (strcasecmp(token, "NONE") == 0) {
            log_levels_enabled = LOG_LEVEL_NONE;
        } else if (strcasecmp(token, "FATAL") == 0) {
            disable ? (log_levels_enabled &= ~LOG_LEVEL_FATAL)
                    : (log_levels_enabled |=  LOG_LEVEL_FATAL);
        } else if (strcasecmp(token, "ERROR") == 0) {
            disable ? (log_levels_enabled &= ~LOG_LEVEL_ERROR)
                    : (log_levels_enabled |=  LOG_LEVEL_ERROR);
        } else if (strcasecmp(token, "WARN") == 0) {
            disable ? (log_levels_enabled &= ~LOG_LEVEL_WARN)
                    : (log_levels_enabled |=  LOG_LEVEL_WARN);
        } else if (strcasecmp(token, "INFO") == 0) {
            disable ? (log_levels_enabled &= ~LOG_LEVEL_INFO)
                    : (log_levels_enabled |=  LOG_LEVEL_INFO);
        } else if (strcasecmp(token, "DEBUG") == 0) {
            disable ? (log_levels_enabled &= ~LOG_LEVEL_DEBUG)
                    : (log_levels_enabled |=  LOG_LEVEL_DEBUG);
        } else if (strcasecmp(token, "TRACE") == 0) {
            disable ? (log_levels_enabled &= ~LOG_LEVEL_TRACE)
                    : (log_levels_enabled |=  LOG_LEVEL_TRACE);
        } else {
            fprintf(stderr, "[LOGGER WARNING] Unknown log level: '%s'\n", token);
        }

        token = strtok(NULL, ",");
    }

    free(input);
}

log_type init_log(const char* filepath, bool enable_colors)
{
    log_levels_enabled = LOG_LEVEL_ALL;
    configure_log_levels_from_env();

    log_output_stream = NULL;
    log_color_auto = true;

    if (!filepath) {
        log_output_stream = stdout;
        log_colors_enabled = enable_colors && isatty(fileno(stdout));
        return LOG_STDOUT;
    }

    log_output_stream = fopen(filepath, "w");
    if (!log_output_stream) {
        fprintf(stderr, "[LOGGER ERROR] Failed to open log file: %s\n", filepath);
        return LOG_ERROR;
    }

    log_colors_enabled = false;
    return LOG_FILE;
}

void shutdown_log(void)
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

void log_set_color_output(bool enabled)
{
    log_color_auto = false;
    log_colors_enabled = enabled;
}

void log_output_ext(log_level level, const char* file, int line, const char* func, const char* msg, ...)
{
    if (!(log_levels_enabled & level)) return;

    if (!log_output_stream) {
        log_output_stream = stdout;
    }

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char timestamp[16];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", t);

    char user_msg[32000];
    va_list args;
    va_start(args, msg);
    vsnprintf(user_msg, sizeof(user_msg), msg, args);
    va_end(args);

    int index = 0;
    switch (level) {
        case LOG_LEVEL_FATAL: index = 0; break;
        case LOG_LEVEL_ERROR: index = 1; break;
        case LOG_LEVEL_WARN:  index = 2; break;
        case LOG_LEVEL_INFO:  index = 3; break;
        case LOG_LEVEL_DEBUG: index = 4; break;
        case LOG_LEVEL_TRACE: index = 5; break;
        default:              index = 0; break;
    }

    fprintf(log_output_stream,
        "%s[%s] [%s] %s:%d (%s): %s%s\n",
        log_colors_enabled ? level_colors[index] : "",
        timestamp,
        level_strings[index],
        file,
        line,
        func,
        user_msg,
        log_colors_enabled ? color_reset : "");

    fflush(log_output_stream);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, int line)
{
    log_output_ext(LOG_LEVEL_FATAL, file, line, "ASSERT", "Assertion failed: %s — %s", expression, message);
}
