#include <cwcore.h>
#include <stdalign.h>
#include <string.h>

#include <cwhost.h>

typedef struct Chunk {
	struct Chunk* next;
} Chunk;

CwPool cwpool_create(CwArena* a, size_t element_size, int size) {
    void* pool = cwalloc(a, element_size, alignof(void*), size);

	intptr_t chunk = (intptr_t)(pool);
    for (int i=0; i < (size - 1); i++, chunk += element_size) {
        ((Chunk*)(chunk)) -> next = (Chunk*)(chunk + element_size); 
    }

    ((Chunk*)(chunk)) -> next = NULL;

    CwPool output;
    output.next_free = pool;
    output.element_size = element_size;
    return output;
}

void* cwpool_alloc(CwPool* self) {
    if (self == NULL || self -> next_free == NULL) return NULL;

	Chunk* output = (Chunk*)(self -> next_free);
	self -> next_free = output -> next;
	return memset(output, 0, self -> element_size);
	// return (void*)(output);
}

void cwpool_free(CwPool* self, void* ptr) {
    if (self == NULL || ptr == NULL) return;

    ((Chunk*)(ptr)) -> next = self -> next_free;
    self -> next_free = ptr;
}
