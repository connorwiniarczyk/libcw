#include <stdint.h>
#include <stdlib.h>


typedef struct CwArray {
	void* ptr;
	size_t element_size;
	size_t size;
	size_t capacity;
} CwArray;

CwArray* cw_array_new(size_t element_size);
void* cw_array_push(CwArray* self);

#ifdef CW_IMPL
CwArray* cw_array_new(size_t element_size) {
    CwArray* self = malloc(sizeof(CwArray));
    if (self == NULL) return NULL;

    self -> element_size = element_size;
    self -> size = 0;
    self -> capacity = 16;
    self -> ptr = malloc(self -> element_size * self -> capacity);
    if (self -> ptr == NULL) return NULL;

    return self;
}

void* cw_array_push(CwArray* self) {
    while (self -> size >= self -> capacity) {
        self -> capacity *= 2;
        self -> ptr = realloc(self -> ptr, self -> capacity * self -> element_size);
    }

    self -> size += 1;
    return (void*)(self -> ptr + (self -> size * self -> element_size));
}

#endif

