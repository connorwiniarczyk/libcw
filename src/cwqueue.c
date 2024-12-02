#include <cwutils/cwqueue.h>

CwQueue* cwqueue_new(CwArena* a, int element_size, int capacity) {
    CwQueue* self = cwnew(a, CwQueue);
    self -> ptr = cwalloc(a, element_size, 8, capacity);
    self -> capacity = capacity;
    self -> element_size = element_size;
    return self;
}

void* cwqueue_push(CwQueue* self) {
    self -> head += 1;
    self -> head %= self -> capacity;

    if (self -> head == self -> tail) return NULL;
    return &self -> ptr[self -> head * self -> element_size];
}

void* cwqueue_pop(CwQueue* self) {
    if (self -> tail == self -> head) return NULL;

    self -> tail += 1;
    self -> tail %= self -> capacity;
    return &self -> ptr[self -> tail * self -> element_size];
}


