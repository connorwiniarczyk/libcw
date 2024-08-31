#ifndef CWFUTURE_H
#define CWFUTURE_H

#include <stdint.h>
#include <cwarray.h>

typedef size_t CwFutureId;

enum CwFutureStatus {
    CWFUTURE_RUNNING,
    CWFUTURE_PAUSED,
    CWFUTURE_SUCCEEDED,
    CWFUTURE_FAILED,
};

typedef struct CwFuture {
    enum CwFutureStatus status;
    int (*poll)(struct CwFuture*);
    void* data;

    // an array of futures awaiting this one,
    // to be resumed once this future has finished
    CwArray* resume_on_finish;
} CwFuture;

typedef struct CwEventHandler {
	size_t event;
	int (*trigger)(void*);
	void* data;
} CwEventHandler;

typedef struct CwEventLoop {
	CwArray* futures;
	CwArray* handlers;
} CwEventLoop;


CwEventLoop* cweventloop_new();
CwFutureId cweventloop_register(CwEventLoop* self, int (*poll)(CwFuture*), void* data);
void cweventloop_update(CwEventLoop* self);

#endif
