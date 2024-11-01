#include <cwutils/cwarray.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

CwArray* cwarray_new(size_t element_size) {
    CwArray* self = malloc(sizeof(CwArray));
    if (self == NULL) return NULL;

    self -> element_size = element_size;
    self -> size = 0;
    self -> capacity = 16;
    self -> ptr = malloc(self -> element_size * self -> capacity);
    if (self -> ptr == NULL) return NULL;

    return self;
}

void* cwarray_get(CwArray* self, size_t index) {
	return self -> ptr + (self -> element_size * index);
}

int cwarray_size(CwArray* self) {
    return self -> size;
}

void* cwarray_push(CwArray* self) {
    while (self -> size >= self -> capacity) {
        self -> capacity *= 2;
        self -> ptr = realloc(self -> ptr, self -> capacity * self -> element_size);
    }

    void* output = (void*)(self -> ptr + (self -> size * self -> element_size));
    self -> size += 1;
    return output;
}

void* cwarray_demote(CwArray* self) {
    void* output = self -> ptr;
    free(self);
    return output;
}

CwArray* cwarray_free(CwArray* self) {
    if (self -> ptr) free(self -> ptr);

    free(self);
    return NULL;
}

CwList* cwlist_new() {
    return (CwList*)(cwarray_new(sizeof(void*)));
}

void cwlist_push_many(CwList* self, size_t n, ...) {
    va_list args;
    va_start(args, n);
    for (size_t i=0; i<n; i++) {
        cwlist_push(self, va_arg(args, void*));
    }
    va_end(args);
}

CwList* cwlist_with_elements(size_t n, ...) {
    CwList* self = cwlist_new();

    va_list args;
    va_start(args, n);
    for (size_t i=0; i<n; i++) {
        cwlist_push(self, va_arg(args, void*));
    }
    va_end(args);

	return self;
}

void cwlist_push(CwList* self, void* item) {
    assert(self -> inner.element_size == sizeof(item));
    memcpy(cwarray_push(&self -> inner), &item, sizeof(item));
}

void* cwlist_get(CwList* self, int index) {
    if (self -> inner.size < index) return NULL;

    return *(void**)(cwarray_get(&self -> inner, index));
}

void cwarray_for_each(CwArray* self, void (*func)(void*)) {
    for (int i=0; i<self -> size; i++) {
        func(cwarray_get(self, i));
    }
}

