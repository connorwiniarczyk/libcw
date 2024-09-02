#ifndef CWFUTURE_H
#define CWFUTURE_H

#include <stdint.h>
#include "cwarray.h"

typedef size_t CwFutureId;
struct CwFuture;
typedef int (PollFn)(int pc, void* data, struct CwFuture* self);

typedef struct CwFuture {
    int pc; // program counter
    PollFn* poll;
    void* data;

    CwArray* on_complete;
} CwFuture;

CwFuture* cwfuture_new(PollFn* poll, void* data);
void cwfuture_poll(CwFuture* self);
void cwfuture_free(CwFuture* self);
void cwfuture_on_complete(CwFuture* self, void(*func)(void*), void* data);

typedef struct CwEventLoop {
	CwArray* futures;
} CwEventLoop;


CwEventLoop* cweventloop_new();
CwFutureId cweventloop_register(CwEventLoop* self, int (*poll)(CwFuture*), void* data);
void cweventloop_update(CwEventLoop* self);

#endif
