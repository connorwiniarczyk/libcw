#include <cwutils/cwarray.h>

void* cwslice_get(CwSlice* self, int index) {
    if (index >= self -> size || index < 0) return NULL;
	return self -> ptr + (self -> element_size * index);
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
