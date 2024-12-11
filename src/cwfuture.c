#include <cwutils/cwfuture.h>
#include <cwutils/cwarray.h>
#include <cwutils/cwtimer.h>
#include <cwutils/cwarena.h>

#include <stddef.h>
#include <stdbool.h>

CwFuture* cwfuture_new(CwArena a, PollFn* poll, void* data) {
    CwFuture* self = cwnew(&a, CwFuture);
    if (self == NULL) return NULL;

    self -> poll  = poll;
    self -> data  = data;
    self -> arena = a;
    self -> pc    = 1;
    self -> err   = 0;

    return self;
}

int cwfuture_poll(CwFuture* self) {
    if (self -> child) {
        int pc = cwfuture_poll(self -> child);
        if (pc == 0) {
            self -> err = self -> child -> err;
            self -> pc += 1;
            self -> child = NULL;
        }
    }

    else {
        self -> pc = self -> poll(self -> pc, self -> data, self);
    }
    return self -> pc;
}

int cwfuture_run(CwFuture* self) {
    while (cwfuture_poll(self) > 0);
    return self -> err;
}

int cwfuture_abort(CwFuture* self, int err) {
    self -> err = err;
    self -> pc = 0;
    return 0;
}

int cwfuture_await(CwFuture* self, CwFuture* target) {
    self -> child = target;
    return self -> pc;
}

int cwfuture_await_new(CwFuture* self, PollFn* poll, void* data) {
	return cwfuture_await(self, cwfuture_new(self -> arena, poll, data));
}

int poll_sequence(int pc, void* data, CwFuture* self) {
    if (pc < 1) return pc;
    if (pc - 1 >= cwarray_size(data)) return 0;

	// else
    return cwfuture_await(self, cwlist_get(data, pc - 1));
}

CwFuture* cwfuture_sequence(CwArena a, CwList* list) {
    return cwfuture_new(a, &poll_sequence, list);
}

static int poll_race(int pc, void* data, CwFuture* self) {
    if (pc != 1) return pc;
    for (int i=0; i < cwarray_size(data); i++) {
        int res = cwfuture_poll(cwlist_get(data, i));

        // if any of the inner futures finishes, end the whole future
        if (res < 1) {
            self -> err = i;
            return 0;
        }
    }

    return 1;
}

CwFuture* cwfuture_race(CwArena a, CwList* list) {
	return cwfuture_new(a, poll_race, list);
}
