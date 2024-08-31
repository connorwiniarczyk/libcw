#include <cwfuture.h>
#include <cwarray.h>
#include <stddef.h>


CwEventLoop* cweventloop_free(CwEventLoop* self) {
    if (self == NULL) return NULL;

    if (self -> futures) cwarray_free(self -> futures);
    if (self -> handlers) cwarray_free(self -> handlers);

    free(self);
    return NULL;
}

CwEventLoop* cweventloop_new() {
    CwEventLoop* self = malloc(sizeof(CwEventLoop));
    if (self == NULL) return NULL;

    *self = (CwEventLoop){0};

    self -> futures = cwarray_new(sizeof(CwFuture));
    if (self -> futures == NULL) return cweventloop_free(self);

    self -> handlers = cwarray_new(sizeof(CwEventHandler));
    if (self -> handlers == NULL) return cweventloop_free(self);

    return self;
}

void cwfuture_init(CwFuture* self, int (*poll)(CwFuture*), void* data) {
    self -> poll = poll;
    self -> data = data;
    self -> status = CWFUTURE_RUNNING;
}

CwFutureId cweventloop_register(CwEventLoop* self, int (*poll)(CwFuture*), void* data) {
    cwfuture_init(cwarray_push(self -> futures), poll, data);
    return self -> futures -> size;
}


void cweventloop_update(CwEventLoop* self) {
	for (size_t i=0;i<self -> futures -> size;i++) {
    	CwFuture* current = cwarray_get(self -> futures, i);
    	if (current == NULL) continue;
    	if (current -> status != CWFUTURE_RUNNING) continue;

    	(current -> poll)(current);
	}
}
