#include <cwcore.h>
#include <cwplatform.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void cwpanic_out_of_memory(const char* msg) {
	fprintf(stderr, "PANIC: %s\n", msg);
	cwplatform_panic();
}

// Shamelessly stolen from: https://nullprogram.com/blog/2023/09/27/
void* cwalloc(CwArena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {

    // optimized equivilant to -1 * (start % align)
	ptrdiff_t padding = -(uintptr_t)(a -> start) & (align - 1);
	ptrdiff_t available = a -> end - a -> start - padding;

	if (available < 0 || count > available / size) {
    	cwpanic_out_of_memory("arena memory overflowed");
	}

	void* output = a -> start + padding;
	a -> start += padding + count * size;
	return memset(output, 0, count * size);
}


CwArena cwarena_empty() {
    return (CwArena) { .start = NULL, .end = NULL };
}

CwArena cwarena_from_buffer(void* buffer, ptrdiff_t size) {
    return (CwArena) {
        .start = (uint8_t*)(buffer),
        .end = buffer + size,
    };
}

CwArena cwarena_create(CwAllocFn* alloc, ptrdiff_t size) {
    CwArena output = {0};
    output.start = alloc(size);
    if (output.start == NULL) cwpanic_out_of_memory("arena initial allocation failed");

    output.end = output.start ? output.start + size : 0;
    return output;
}

CwArena cwarena_reserve(CwArena* self, ptrdiff_t size) {
    CwArena output;
    output.start = cwalloc(self, size, 1, 1);
    output.end = output.start + size;
    return output;
}
