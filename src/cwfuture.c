#include <cwutils/cwfuture.h>
#include <cwutils/cwarray.h>
#include <stddef.h>
#include <stdbool.h>

typedef void (HandlerFn)(void* data);
typedef struct Handler {
    HandlerFn* fn;
    void* data;
} Handler;

static void handler_call(void* data) {
	Handler* self = (Handler*)(data);
	(self -> fn)(self -> data);
}

static void handler_init(Handler* self, HandlerFn* fn, void* data) {
    self -> fn = fn;
    self -> data = data;
}

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
    // if (self -> timeout) {
    //     if (cwfuture_poll(self -> timeout) == 0) {
    //         self -> pc = CWFUTURE_FAILURE;
    //         return self -> pc;
    //     }
    // }

    if (self -> child.future) {
        int pc = cwfuture_poll(self -> child.future);
        if (pc > 0) return pc;

        else if (pc == CWFUTURE_SUCCESS) self -> pc += 1;
        else if (pc == CWFUTURE_FAILURE) self -> pc = self -> child.catch;

        // self -> child.future = cwfuture_free(self -> child.future);
        self -> child.future = NULL;
        return self -> pc;
    }

    self -> pc = self -> poll(self -> pc, self -> data, self);
    if (self -> pc == 0 && self -> on_success) {
        cwarray_for_each(self -> on_success, handler_call);
    }

    return self -> pc;
}

int cwfuture_block_on(CwFuture* self) {
    while (cwfuture_poll(self) > 0);
    int output = self -> pc;
    // cwfuture_free(self);
    return output;
}

static int poll_timeout(int pc, void* data, CwFuture* self) {

}

CwFuture* cwfuture_with_timeout(CwFuture* self, int ms) {

}

// void cwfuture_abort_on(CwFuture* self, CwFuture* target) {
//     CwList* list = cwlist_new();
//     cwlist_push(list, self);
//     cwlist_push(list, target);
//     self -> timeout = target;
// }


int cwfuture_await_with_catch(CwFuture* self, CwFuture* target, int catch) {
    self -> child.future = target;
    self -> child.catch = catch;
    return self -> pc;
}

int cwfuture_await(CwFuture* self, CwFuture* target) {
    return cwfuture_await_with_catch(self, target, -1);
}

CwFuture* cwfuture_free(CwFuture* self) {
    // if (self -> timeout) cwfuture_free(self -> timeout);

    if (self -> on_cleanup) {
        cwarray_for_each(self -> on_cleanup, handler_call);
        self -> on_cleanup = cwarray_free(self -> on_cleanup);
    }

    if (self -> on_success) cwarray_free(self -> on_success);

    free(self);
    return NULL;
}

void cwfuture_on_cleanup(CwFuture* self, HandlerFn* fn, void* data) {
    if (self -> on_cleanup == NULL) self -> on_cleanup = cwarray_new(sizeof(Handler));
    handler_init(cwarray_push(self -> on_cleanup), fn, data);
}

void cwfuture_on_success(CwFuture* self, HandlerFn* fn, void* data) {
    if (self -> on_success == NULL) self -> on_success = cwarray_new(sizeof(Handler));
    handler_init(cwarray_push(self -> on_success), fn, data);
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
    (void)(self);
    if (pc != 1) return pc;
    for (size_t i=0; i < cwarray_size(data); i++) {
        int res = cwfuture_poll(cwlist_get(data, i));

        // if any of the inner futures finishes, end the whole future
        if (res < 1) return res;
    }

    return 1;
}

CwFuture* cwfuture_race(CwList* list) {
	return cwfuture_new(poll_race, list);
}
