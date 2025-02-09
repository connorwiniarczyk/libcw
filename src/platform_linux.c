#include <cwcore.h>
#include <cwhost.h>

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>
#include <sys/time.h>

void cwhost_exit(int code) { exit(code); }
void cwhost_sleepms(int ms) { usleep(ms * 1000); }

int64_t cwhost_get_realtime() {
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec;
}

int64_t cwhost_get_monotime() {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return now.tv_sec;
}

typedef struct Timeout {
	struct timeval start;
	int64_t duration_ms;
} Timeout;

int poll_timeout_ms(int pc, void* data, CwFuture* f) {
    (void)(f);
    if (pc != 1) return 0;

    struct timeval now, diff;
    gettimeofday(&now, NULL);

	Timeout* self = (Timeout*)(data);
    timersub(&now, &self -> start, &diff);
    int64_t elapsed_ms = (diff.tv_sec * 1000) + (diff.tv_usec / 1000);

	return elapsed_ms > self -> duration_ms ? 0 : 1; 
}

CwFuture* cwtimeout_ms(CwArena a, int64_t ms) {
    Timeout* self = cwnew(&a, Timeout);
    self -> duration_ms = ms;
    gettimeofday(&self -> start, NULL);

    return cwfuture_new(a, poll_timeout_ms, self);
}

CwStr cwhost_read_file(CwArena* a, const char* path) {
	CwStr output;
	output.ptr = (char*)(a -> start);

    FILE* file = fopen(path, "rb");
    if (file == NULL) return cwstr_empty();

    output.ptr = (char*)(a -> start);
	size_t bytes_read;
	while ((bytes_read = fread(cwalloc(a, 1, 1, 1), 1, 1, file)) == 1) output.size += bytes_read;

	fclose(file);
	return output;
}

int cwhost_append_file(const char* path, CwStr self) {
    FILE* file = fopen(path, "ab");
    if (file == NULL) return 1;

    int bytes_written = fwrite(self.ptr, 1, self.size, file);
    if (bytes_written != self.size) return 2;

    return 0;
}

int cwhost_write_file(const char* path, CwStr self) {
    FILE* file = fopen(path, "wb");
    if (file == NULL) return 1;

    int bytes_written = fwrite(self.ptr, 1, self.size, file);
    if (bytes_written != self.size) return 2;

    return 0;
}

// -- Logging Stuff --

static CwLogger logger = {0};

static void print_logger(void* data, const char* message) {
    (void)(data);
    write(2, message, strlen(message));
	write(2, "\n", 1);
}

void cwlogger_init(CwArena* a, int buffer_size) {
	logger.log = print_logger;
	logger.data = NULL;
	logger.fmt_buffer = cwarena_reserve(a, buffer_size);
}

void cwlogger_set(void (*log)(void* data, const char* message), void* data) {
	logger.log = log;
	logger.data = data;
}

void cwlog(const char* fmt, ...) {
    CwArena scratch = logger.fmt_buffer;
    if (logger.log == NULL) return;

    va_list args;
    va_start(args, fmt);
	CwStr message = cwfmtV(&scratch, fmt, args);
    va_end(args);

    logger.log(logger.data, message.ptr);
}

void cwlog_error_handler(const char* file, int line, const char* fmt, ...) {
    CwArena scratch = logger.fmt_buffer;
    if (logger.log == NULL) return;

    CwStr prefix = cwfmt(&scratch, "ERROR (%s line %d)", file, line);

    va_list args;
    va_start(args, fmt);
	CwStr message = cwfmtV(&scratch, fmt, args);
    va_end(args);

    // write(2, prefix.ptr, prefix.size);
    // write(2, message.ptr, message.size);

    CwStr output = cwfmt(&scratch, "%w %w", prefix, message);

    // write(2, output.ptr, output.size);
    logger.log(logger.data, output.ptr);
}

void cwsleep_ms(int ms) {
    usleep(ms * 1000);
}

CwCmd cwcmd_create(CwArena a, const char* cmd) {
    CwCmd output;
    output.ptr = cwnew(&a, char*);
    output.ptr[0] = cmd;
    output.size = 1;
    output.mem = a;

    output.io.in  = 0;
    output.io.out = 1;
    output.io.err = 2;

    return output;
}

void cwcmd_push_arg(CwCmd* self, const char* arg) {
    *(const char**)(cwnew(&self -> mem, char*)) = (char*)(arg);
}

int cwcmd_run(CwCmd* self) {
    // terminate the array with a NULL pointer
    if (self -> ptr[self -> size - 1] != NULL) {
    	(void)(cwnew(&self -> mem, char*));
    }

	const char* name = self -> ptr[0];
	char* environment[] = { NULL };

	int pid = fork();
	if (pid == 0) {
    	if (self -> io.in != 0)  dup2(self -> io.in,  0);
    	if (self -> io.out != 0) dup2(self -> io.out, 1);
    	if (self -> io.err != 0) dup2(self -> io.err, 2);

    	int err = execve(name, (char* const*)(self -> ptr), environment);
    	return err;
	} 

	else {
    	int status;
    	do {
        	waitpid(pid, &status, 0);
    	} while (WIFEXITED(status) == false);

    	return WEXITSTATUS(status);
	}
}
