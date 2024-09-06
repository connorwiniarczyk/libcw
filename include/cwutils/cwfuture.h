#ifndef CWFUTURE_H
#define CWFUTURE_H

#include <stdint.h>
#include <stdbool.h>
#include "cwarray.h"

typedef size_t CwFutureId;
struct CwFuture;
typedef int (PollFn)(int pc, void* data, struct CwFuture* self);

enum CwFutureStatus {
    CWFUTURE_SUCCESS =  0,
    CWFUTURE_FAILURE = -1,
};

typedef struct CwFuture {
    int pc; // program counter
    PollFn* poll;
    void* data;

    CwArray* on_success;
    CwArray* on_cleanup;

    struct { struct CwFuture* future; int catch; } awaiting;
    struct CwFuture* timeout;
} CwFuture;

CwFuture* cwfuture_new(PollFn* poll, void* data);
CwFuture* cwfuture_free(CwFuture* self);

int cwfuture_poll(CwFuture* self);

void cwfuture_on_success(CwFuture* self, void(*func)(void*), void* data);
void cwfuture_on_cleanup(CwFuture* self, void(*func)(void*), void* data);
// void cwfuture_on_fail(CwFuture* self, void(*func)(void*), void* data);

int cwfuture_await(CwFuture* self, CwFuture* target, int catch);
void cwfuture_abort_on(CwFuture* self, CwFuture* target);

typedef struct CwEventLoop {
	CwArray* futures;
} CwEventLoop;


CwEventLoop* cweventloop_new();
CwFutureId cweventloop_register(CwEventLoop* self, int (*poll)(CwFuture*), void* data);
void cweventloop_update(CwEventLoop* self);

#endif
