#include <stdint.h>
#include <stdlib.h>

typedef struct CwArray {
	void* ptr;
	size_t element_size;
	size_t size;
	size_t capacity;
} CwArray;

CwArray* cwarray_new(size_t element_size);
void* cwarray_push(CwArray* self);
void* cwarray_get(CwArray* self, size_t index);
void* cwarray_demote(CwArray* self);

#ifdef CWARRAY_IMPLEMENTATION
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

#endif
