#include <cwutils/cwfuture.h>
#include <cwutils/cwarray.h>
#include <cwutils/cwtimer.h>
#include <cwutils/cwarena.h>

#include <stddef.h>
#include <stdbool.h>

// void cwfuture_init(CwFuture* self, PollFn* poll, void* data) {
//     *self = (CwFuture){0};
//     self -> poll = poll;
//     self -> data = data;
//     self -> pc = 1;
// }

CwFuture* cwfuture_new(CwArena* a, PollFn* poll, void* data) {
    CwFuture* self = cwalloc(a, sizeof(CwFuture), sizeof(CwFuture), 1);
    if (self == NULL) return NULL;

    self -> poll  = poll;
    self -> data  = data;
    self -> arena = *a;
    self -> pc    = 1;
    self -> err   = 0;

    return self;
}

int cwfuture_poll(CwFuture* self) {
    if (self -> child.future) {
        int pc = cwfuture_poll(self -> child.future);
        if (pc > 0) return self -> pc;

        else if (self -> child.future -> err == 0) self -> pc += 1;
        else if (self -> child.future -> err) self -> pc = self -> child.catch;

        self -> child.future = NULL;
        return self -> pc;
    }

    self -> pc = self -> poll(self -> pc, self -> data, self);

    return self -> pc;
}

int cwfuture_run(CwFuture* self) {
    while (cwfuture_poll(self) > 0);

    return self -> err;
}

int cwfuture_await_with_catch(CwFuture* self, CwFuture* target, int catch) {
    self -> child.future = target;
    self -> child.catch = catch;
    return self -> pc;
}

int cwfuture_await(CwFuture* self, CwFuture* target) {
    return cwfuture_await_with_catch(self, target, -1);
}

int poll_sequence(int pc, void* data, CwFuture* self) {
    if (pc < 1) return pc;
    if ((size_t)(pc - 1) >= cwarray_size(data)) return 0;

	// else
    return cwfuture_await(self, cwlist_get(data, pc - 1));
}

CwFuture* cwfuture_sequence(CwArena* a, CwList* list) {
    return cwfuture_new(a, &poll_sequence, list);
}

static int poll_race(int pc, void* data, CwFuture* self) {
    if (pc != 1) return pc;
    for (size_t i=0; i < cwarray_size(data); i++) {
        int res = cwfuture_poll(cwlist_get(data, i));

        // if any of the inner futures finishes, end the whole future
        if (res < 1) {
            self -> err = i;
            return res;
        }
    }

    return 1;
}

CwFuture* cwfuture_race(CwArena* a, CwList* list) {
	return cwfuture_new(a, poll_race, list);
}
