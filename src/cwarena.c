#include <cwutils/cwarena.h>
#include <string.h>
#include <stdlib.h>

// Shamelessly stolen from: https://nullprogram.com/blog/2023/09/27/
void* cwalloc(CwArena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {

    // optimized equivilant to -1 * (start % align)
	ptrdiff_t padding = -1 * ((uintptr_t)(a -> start) & (align - 1));
	ptrdiff_t available = (a -> end - a -> start) - padding;
	if (available < 0 || count > available / size) return NULL;

	void* output = a -> start + padding;
	a -> start += padding + count * size;
	return memset(output, 0, count * size);
}

CwArena cwarena_new(ptrdiff_t size) {
    CwArena output = {0};
    output.start = malloc(size);
    output.end = output.start ? output.start + size : 0;
    return output;
}
