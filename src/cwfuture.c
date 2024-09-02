#include <cwfuture.h>
#include <cwarray.h>
#include <stddef.h>

typedef void (HandlerFn)(void* data);
typedef struct Handler {
    HandlerFn* fn;
    void* data;
} Handler;

CwFuture* cwfuture_new(PollFn* poll, void* data) {
    CwFuture* self = malloc(sizeof(CwFuture));
    if (self == NULL) return NULL;

    *self = (CwFuture){0};
    self -> poll = poll;
    self -> data = data;
    self -> pc = 1;

    return self;
}

void cwfuture_poll(CwFuture* self) {
    self -> pc = self -> poll(self -> pc, self -> data, self);

    if (self -> pc == 0 && self -> on_complete) {
		for (size_t i=0; i<self -> on_complete -> size; i++) {
    		Handler* h = cwarray_get(self -> on_complete, i);
    		h -> fn(h -> data);
		}
    }
}

void cwfuture_free(CwFuture* self) {
    if (self -> on_complete) cwarray_free(self -> on_complete);
    free(self);
}


void cwfuture_on_complete(CwFuture* self, HandlerFn* fn, void* data) {
    if (self -> on_complete == NULL) self -> on_complete = cwarray_new(sizeof(Handler));

    Handler* next = (Handler*)(cwarray_push(self -> on_complete));
    next -> fn = fn;
    next -> data = data;
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
