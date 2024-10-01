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

    struct { struct CwFuture* future; int catch; } child;
} CwFuture;

void cwfuture_init(CwFuture* self, PollFn* poll, void* data);

CwFuture* cwfuture_new(PollFn* poll, void* data);
CwFuture* cwfuture_free(CwFuture* self);


int cwfuture_poll(CwFuture* self);
int cwfuture_block_on(CwFuture* self);

void cwfuture_on_success(CwFuture* self, void(*func)(void*), void* data);
void cwfuture_on_cleanup(CwFuture* self, void(*func)(void*), void* data);

int cwfuture_await(CwFuture* self, CwFuture* target);
int cwfuture_await_with_catch(CwFuture* self, CwFuture* target, int catch);
void cwfuture_abort_on(CwFuture* self, CwFuture* target);
void cwfuture_abort(CwFuture* self);

// CwFutureList* cwfuture_list_new();
// void cwfuture_list_push(CwFutureList* self, CwFuture* item);
// CwFuture* cwfuture_list_get(CwFutureList* self, size_t index);

CwFuture* cwfuture_race(CwList* list);
CwFuture* cwfuture_all(CwList* list);
CwFuture* cwfuture_sequence(CwList* list);

#endif
