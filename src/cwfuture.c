#include <cwfuture.h>
#include <cwarray.h>
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

CwFuture* cwfuture_new(PollFn* poll, void* data) {
    CwFuture* self = malloc(sizeof(CwFuture));
    if (self == NULL) return NULL;

    *self = (CwFuture){0};
    self -> poll = poll;
    self -> data = data;
    self -> pc = 1;

    return self;
}

int cwfuture_poll(CwFuture* self) {
    if (self -> timeout) {
        if (cwfuture_poll(self -> timeout) == 0) {
            self -> pc = CWFUTURE_FAILURE;
            return self -> pc;
        }
    }

    if (self -> awaiting.future) {
        int pc = cwfuture_poll(self -> awaiting.future);
        if (pc > 0) return pc;

        else if (pc == CWFUTURE_SUCCESS) self -> pc += 1;
        else if (pc == CWFUTURE_FAILURE) self -> pc = self -> awaiting.catch;

        self -> awaiting.future = cwfuture_free(self -> awaiting.future);
        return self -> pc;
    }

    self -> pc = self -> poll(self -> pc, self -> data, self);
    if (self -> pc == 0 && self -> on_success) {
        cwarray_for_each(self -> on_success, handler_call);
    }

    return self -> pc;
}

void cwfuture_abort_on(CwFuture* self, CwFuture* target) {
    self -> timeout = target;
}



int cwfuture_await(CwFuture* self, CwFuture* target, int catch) {
    self -> awaiting.future = target;
    self -> awaiting.catch = catch;
    return self -> pc;
}

CwFuture* cwfuture_free(CwFuture* self) {
    if (self -> timeout) cwfuture_free(self -> timeout);

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

CwEventLoop* cweventloop_free(CwEventLoop* self) {
    if (self == NULL) return NULL;
    if (self -> futures) cwarray_free(self -> futures);

    free(self);
    return NULL;
}

CwEventLoop* cweventloop_new() {
    CwEventLoop* self = malloc(sizeof(CwEventLoop));
    if (self == NULL) return NULL;

    *self = (CwEventLoop){0};

    self -> futures = cwarray_new(sizeof(CwFuture));
    if (self -> futures == NULL) return cweventloop_free(self);

    return self;
}

// CwFutureId cweventloop_register(CwEventLoop* self, int (*poll)(CwFuture*), void* data) {
//     CwFuture
//     CwFuture* next;

// }


// void cweventloop_update(CwEventLoop* self) {
// 	for (size_t i=0;i<self -> futures -> size;i++) {
//     	CwFuture* current = cwarray_get(self -> futures, i);
//     	if (current == NULL) continue;
//     	if (current -> status != CWFUTURE_RUNNING) continue;

//     	(current -> poll)(current);
// 	}
// }
