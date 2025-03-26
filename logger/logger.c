#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>	/* variadics */

void report_assertion_failure(const char *expression, const char *message, const char *file, int line)
{
	log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}

int initialize_logging(const char* filepath)
{
	if (!filepath) {
		log_output_stream = stdout;
		return 2;
	}

	log_output_stream = fopen(filepath, "w");
	if (!log_output_stream) {
		return 0; // failed to open
	}
	return 1;
}

void shutdown_logging()
{
    if (log_output_stream && log_output_stream != stderr) {
        fclose(log_output_stream);
    }
    log_output_stream = NULL;
}

void log_output(log_level level, const char *msg, ...)
{
	const char *level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ","[INFO]: ","[DEBUG]: ","[TRACE]: "};

	char buffer_message[32000];

	va_list arg_ptr;
	va_start(arg_ptr, msg);

	vsnprintf(buffer_message, 32000, msg, arg_ptr);
	va_end(arg_ptr);

	char out_message[32000];
	memset(out_message, 0, sizeof out_message);
	sprintf(out_message, "%s%s\n", level_strings[level], buffer_message);

    fprintf(log_output_stream, "%s", out_message);
}

