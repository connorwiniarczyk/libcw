#include <cwutils/cwarray.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

void* cwslice_get(CwSlice* self, int index) {
    if (index >= self -> size || index < 0) return NULL;
	return self -> ptr + (self -> element_size * index);
}

CwArray cwarray_new(CwArena a, int element_size) {
    return (CwArray) {
        .ptr = cwalloc(&a, 1, 4, 0),
        .size = 0,
        .element_size = element_size,
        .a = a,
    };
}

void* cwarray_push(CwArray* self) {
    void* output = cwalloc(&self -> a, self -> element_size, 4, 1);
    self -> size += 1;

    return output;
}

CwSlice cwarray_finish(CwArray* self, CwArena* a) {
    *a = self -> a;

    return (CwSlice) {
        .ptr = self -> ptr,
        .size = self -> size,
        .element_size = self -> element_size,
    };
}

void cwarray_clear(CwArray* self) {
    self -> a.start = self -> ptr;
    self -> size = 0;
}

int cwarray_size(CwArray* self) {
    return self -> size;
}



CwList cwlist_new(CwArena a) {
    CwList output;
    output.inner = cwarray_new(a, sizeof(void*));
    return output;
}

void cwlist_push(CwList* self, void* item) {
    assert(self -> inner.element_size == sizeof(void*));
    *(void**)(cwarray_push((CwArray*)(self))) = item;

}

void* cwlist_get(CwList* self, int index) {
    assert(self -> inner.element_size == sizeof(void*));
    if (index >= self -> inner.size) return NULL;

    return *(void**)(cwslice_get((CwSlice*)(self), index));
}

void cwlist_push_many(CwList* self, size_t n, ...) {
    va_list args;
    va_start(args, n);
    for (size_t i=0; i<n; i++) {
        cwlist_push(self, va_arg(args, void*));
    }
    va_end(args);
}

CwList cwlist_with_elements(CwArena a, size_t n, ...) {
    CwList self = cwlist_new(a);

    va_list args;
    va_start(args, n);
    for (size_t i=0; i<n; i++) {
        cwlist_push(&self, va_arg(args, void*));
    }
    va_end(args);

	return self;
}

CwRingBuffer cwringbuffer_new(CwArena* a, int element_size, int size) {
	CwRingBuffer output;
	output.data.ptr = cwalloc(a, element_size, 4, size);
	output.data.element_size = element_size;
	output.data.size = size;
	output.front = 0;
	output.back = 0;

	return output;
}

void* cwringbuffer_push(CwRingBuffer* self) {
    int next = (self -> back + 1) % self -> data.size;

    if (next == self -> front) (void)cwringbuffer_next(self); 

    self -> back = next;
    return cwslice_get(&self -> data, self -> back);
}

void* cwringbuffer_next(CwRingBuffer* self) {
    if (self -> front == self -> back) return NULL;

    self -> front += 1;
    self -> front %= self -> data.size;

    void* output = cwslice_get(&self -> data, self -> front);
    return output;
}

void* cwringbuffer_peek(CwRingBuffer* self) {
    return cwslice_get(&self -> data, self -> front);
}



// void cwarray_for_each(CwArray* self, void (*func)(void*)) {
//     for (int i=0; i<self -> size; i++) {
//         func(cwarray_get(self, i));
//     }
// }

