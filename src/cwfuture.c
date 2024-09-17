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

    if (self -> child.future) {
        int pc = cwfuture_poll(self -> child.future);
        if (pc > 0) return pc;

        else if (pc == CWFUTURE_SUCCESS) self -> pc += 1;
        else if (pc == CWFUTURE_FAILURE) self -> pc = self -> child.catch;

        self -> child.future = cwfuture_free(self -> child.future);
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
    cwfuture_free(self);
    return output;
}

void cwfuture_abort_on(CwFuture* self, CwFuture* target) {
    self -> timeout = target;
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

CwFutureList* cwfuture_list_new() {
    return (CwFutureList*)(cwarray_new(sizeof(CwFuture*)));
}
void cwfuture_list_push(CwFutureList* self, CwFuture* item) {
   	void* next = cwarray_push(&self -> inner);
   	*(CwFuture**)(next) = item;
}

CwFuture* cwfuture_list_get(CwFutureList* self, size_t index) {
    return *(CwFuture**)(cwarray_get(&self -> inner, index));
}

static int poll_race(int pc, void* data, CwFuture* self) {
    (void)(self);
    if (pc != 1) return pc;

    CwFutureList* list = (CwFutureList*)(data);
    for (size_t i=0; i<list -> inner.size; i++) {
        int res = cwfuture_poll(cwfuture_list_get(list, i));
        if (res == 0) return 0; 
    }

    // else
    return 1;
}


CwFuture* cwfuture_race(CwFutureList* list) {
	return cwfuture_new(poll_race, list);
}
