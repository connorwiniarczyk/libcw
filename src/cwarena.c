#include <cwcore.h>
#include <cwhost.h>

#ifdef CWNOSTDLIB
void* memset(void* dest, int val, size_t len) {
    uint8_t* ptr = (uint8_t*)(dest);
    while (len-- > 0) {
        *ptr++ = val;
    }

    return dest;
}
#endif

void* cwarena_align_to(CwArena* a, ptrdiff_t align) {
	ptrdiff_t padding = -(uintptr_t)(a -> start) & (align - 1);
	ptrdiff_t available = a -> end - a -> start - padding;
	if (available < 0) cwpanic("arena memory overflowed");

	a -> start += padding;
	return a -> start;
}

// Shamelessly stolen from: https://nullprogram.com/blog/2023/09/27/
void* cwalloc(CwArena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {

    // optimized equivilant to -1 * (start % align)
	ptrdiff_t padding = -(uintptr_t)(a -> start) & (align - 1);
	ptrdiff_t available = a -> end - a -> start - padding;

	if (available < 0 || count > available / size) {
        cwpanic("arena memory overflowed");
	}

	void* output = a -> start + padding;
	a -> start += padding + count * size;
	return memset(output, 0, count * size);
}

void* cwarena_push_byte(CwArena* self, uint8_t byte) {
	uint8_t* next = cwalloc(self, 1, 1, 1);
	*next = byte;
	return next;
}

void* cwarena_push_ptr(CwArena* self, const void* ptr) {
	void** next = cwalloc(self, sizeof(void*), alignof(void*), 1);
	*next = (void*)ptr;
	return next;
}

void* cwarena_push_list(CwArena* a, const void** src) {
    void* output = cwarena_align_to(a, alignof(void**));
    for (int i=0; src[i] != NULL; i++) {
        cwarena_push_ptr(a, src[i]);
    }
	return output;
}

int cwarena_allocated(CwArena self, void* start) {
    return (intptr_t)(self.start) - (intptr_t)(start);
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
    if (output.start == NULL) cwpanic("arena initial allocation failed");

    output.end = output.start ? output.start + size : 0;
    return output;
}

CwArena cwarena_reserve(CwArena* self, ptrdiff_t size) {
    CwArena output;
    output.start = cwalloc(self, size, 1, 1);
    output.end = output.start + size;
    return output;
}

CwArena cwarena_reserve_all(CwArena* self) {
    return cwarena_reserve(self, cwarena_remaining(*self));
}

int cwarena_remaining(CwArena self) {
	return (intptr_t)(self.end) - (intptr_t)(self.start);
}

// array stuff

int cwarray_len(CwArray self) {
    int size = (intptr_t)(self.a.start) - (intptr_t)(self.ptr);
    if (size % self.stride != 0) cwpanic("invalid array length");

    return size / self.stride;
}

CwArray cwarray(void* ptr, int stride, CwArena a) {
    return (CwArray){ ptr, stride, a };
}

CwArray cwarray_clone(CwArena* a, CwArray self) {
    const int align = 8;
    CwArray output;
    output.ptr = cwarena_align_to(a, align);

	intptr_t src = (intptr_t)(self.ptr);
    while ((intptr_t)(self.ptr) < (intptr_t)(self.a.start)) {
        void* next = cwalloc(a, self.stride, align, 1);
        memcpy(next, self.ptr, self.stride);

        src += self.stride;
        self.ptr = (void*)(src);
    }

    output.stride = self.stride;
    output.a = cwarena_empty();

    return output;
}
