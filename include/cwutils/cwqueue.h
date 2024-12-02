#ifndef CWQUEUE_H
#define CWQUEUE_H

#include <cwutils/cwarena.h>

#include <stdint.h>

typedef struct CwQueue {
    uint8_t* ptr;
    ptrdiff_t capacity;
    ptrdiff_t element_size;
    int head;
    int tail;
} CwQueue;

CwQueue* cwqueue_new(CwArena* a, int element_size, int capacity);
void* cwqueue_push(CwQueue* self);
void* cwqueue_pop(CwQueue* self);

#endif
