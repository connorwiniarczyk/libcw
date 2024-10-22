#include <cwutils/cwfuture.h>
#include <cwutils/cwarray.h>
#include <cwutils/cwtimer.h>
#include <stddef.h>
#include <stdbool.h>

typedef void (CallbackFn)(void* data);

void cwfuture_init(CwFuture* self, PollFn* poll, void* data) {
    *self = (CwFuture){0};
    self -> poll = poll;
    self -> data = data;
    self -> pc = 1;
}

CwFuture* cwfuture_new(PollFn* poll, void* data) {
    CwFuture* self = malloc(sizeof(CwFuture));
    if (self == NULL) return NULL;

	cwfuture_init(self, poll, data);
    return self;
}

int cwfuture_poll(CwFuture* self) {
    if (self -> child.future) {
        int pc = cwfuture_poll(self -> child.future);
        if (pc > 0) return pc;

        else if (self -> child.future -> err == 0) self -> pc += 1;
        else if (self -> child.future -> err) self -> pc = self -> child.catch;

        self -> child.future = NULL;
        return self -> pc;
    }

    self -> pc = self -> poll(self -> pc, self -> data, self);

    CallbackFn* on_success = self -> on_success.callback;
    if (self -> pc == 0 && on_success) on_success(self -> on_success.data);

    return self -> pc;
}

int cwfuture_block_on(CwFuture* self) {
    while (cwfuture_poll(self) > 0);
    // cwfuture_free(self);
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

CwFuture* cwfuture_free(CwFuture* self) {
    CallbackFn* cleanup = self -> on_cleanup.callback;
    if (cleanup) cleanup(self -> on_cleanup.data);
    // free(self);
    return NULL;
}

void cwfuture_on_cleanup(CwFuture* self, CallbackFn* fn, void* data) {
    self -> on_cleanup.callback = fn;
    self -> on_cleanup.data = data;

}

void cwfuture_on_success(CwFuture* self, CallbackFn* fn, void* data) {
    self -> on_success.callback = fn;
    self -> on_success.data = data;
}

int poll_sequence(int pc, void* data, CwFuture* self) {
    if (pc < 1) return pc;
    if ((size_t)(pc - 1) >= cwarray_size(data)) return 0;

	// else
    return cwfuture_await(self, cwlist_get(data, pc - 1));
}

CwFuture* cwfuture_sequence(CwList* list) {
    return cwfuture_new(&poll_sequence, list);
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

CwFuture* cwfuture_race(CwList* list) {
	return cwfuture_new(poll_race, list);
}
