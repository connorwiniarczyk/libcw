#ifndef CWFUTURE_H
#define CWFUTURE_H

#include <stdint.h>
#include <stdbool.h>

#include "cwarena.h"
#include "cwarray.h"

struct CwFuture;
typedef int (PollFn)(int pc, void* data, struct CwFuture* self);
typedef struct CwFuture {
    int pc; // program counter
    int err;

    PollFn* poll;
    void*   data;
    CwArena arena;

    struct CwFuture* child;

} CwFuture;

CwFuture* cwfuture_new(CwArena a, PollFn* poll, void* data);

int cwfuture_poll(CwFuture* self);
int cwfuture_run(CwFuture* self);
int cwfuture_abort(CwFuture* self, int err);
int cwfuture_await(CwFuture* self, CwFuture* target);
int cwfuture_await_new(CwFuture* self, PollFn* poll, void* data);

// CwFuture* cwfuture_race(CwArena a, CwList* list);
// CwFuture* cwfuture_all(CwArena a, CwList* list);
// CwFuture* cwfuture_sequence(CwArena a, CwList* list);

#endif
