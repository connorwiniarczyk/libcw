#include <cwcore.h>
#include <cwhost.h>

#include <unistd.h>
#include <stdalign.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>

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


void cwsleep_ms(int ms) {
    usleep(ms * 1000);
}


CwPipe cwhost_make_pipe() {
    CwPipe output = { .input = -1, .output = -1 };

    int pipe_fds[2];
    int err = pipe(pipe_fds);
    if (err) return output;

    output.input = pipe_fds[1];
    output.output = pipe_fds[0];

    return output;
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
    self -> size += 1;
}


void cwcmd_push_arglist(CwCmd* self, const char** arglist) {
    for (int i=0; arglist[i] != NULL; i++) {
        cwcmd_push_arg(self, arglist[i]);
    }
}

int cwcmd_run(CwCmd* self) {
    // terminate the array with a NULL pointer
    if (self -> ptr[self -> size - 1] != NULL) {
    	(void)(cwnew(&self -> mem, char*));
    }

	const char* name = self -> ptr[0];

	int pid = fork();
	if (pid == 0) {
    	if (self -> io.in  != 0) dup2(self -> io.in,  0);
    	if (self -> io.out != 1) dup2(self -> io.out, 1);
    	if (self -> io.err != 2) dup2(self -> io.err, 2);

    	int err = execvp(name, (char* const*)(self -> ptr));
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
