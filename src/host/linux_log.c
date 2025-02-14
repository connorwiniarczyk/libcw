#include <cwcore.h>
#include <cwhost.h>
#include <string.h>
#include <unistd.h>

// -- Logging Stuff --

static CwLogger logger = {0};

static void print_logger(void* data, CwStr message) {
    (void)(data);
    write(2, message.ptr, message.size);
	write(2, "\n", 1);
}

void cwlogger_init(CwArena* a, int buffer_size) {
	logger.log = print_logger;
	logger.data = NULL;
	logger.fmt_buffer = cwarena_reserve(a, buffer_size);
}

void cwlogger_set(void (*log)(void* data, CwStr message), void* data) {
	logger.log = log;
	logger.data = data;
}

void cwlog(const char* fmt, ...) {
    CwArena scratch = logger.fmt_buffer;
    if (logger.log == NULL) return;

    va_list args;
    va_start(args, fmt);
	CwStr message = cwfmt_vargs(&scratch, fmt, args);
    va_end(args);

    logger.log(logger.data, message);
}

void cwlog_error_handler(const char* file, int line, const char* fmt, ...) {
    CwArena scratch = logger.fmt_buffer;
    if (logger.log == NULL) return;

    CwStr prefix = cwfmt(&scratch, "ERROR (%s line %d)", file, line);

    va_list args;
    va_start(args, fmt);
	CwStr message = cwfmt_vargs(&scratch, fmt, args);
    va_end(args);

    CwStr output = cwfmt(&scratch, "%w %w", prefix, message);
    logger.log(logger.data, output);
}
