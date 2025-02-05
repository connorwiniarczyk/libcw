#ifndef CWARRAY_H
#define CWARRAY_H

#include <stdint.h>
#include <stdalign.h>
#include <cwutils/cwarena.h>

#define cwarray_type(t) struct cwarray_ ## t { t* ptr; int size; CwArena mem; }

#define cwarray_init(arr, type, arena) do { \
    arr.ptr = (void*)(cwalloc(&(arena), sizeof(type), alignof(type), 0));   \
    arr.size = 0;                     \
    arr.mem = arena;                  \
} while(0)

#define cwarray_push(arr, type, value) do {  \
    *(type*)(cwnew(&arr.mem, type)) = value; \
    arr.size += 1;                           \
} while(0)

typedef struct CwSlice {
    void* ptr;
    int size;
    int element_size;
} CwSlice;

void* cwslice_get(CwSlice* self, int index);

typedef struct CwRingBuffer {
    CwSlice data;
    int front;
    int back;
} CwRingBuffer;

CwRingBuffer cwringbuffer_new(CwArena* a, int element_size, int size);
void* cwringbuffer_push(CwRingBuffer* self);
void* cwringbuffer_next(CwRingBuffer* self);
void* cwringbuffer_peek(CwRingBuffer* self);

#endif
