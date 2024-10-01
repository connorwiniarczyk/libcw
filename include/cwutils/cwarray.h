#ifndef CWARRAY_H
#define CWARRAY_H

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

size_t cwarray_size(CwArray* self);

CwArray* cwarray_free(CwArray* self);

void cwarray_for_each(CwArray* self, void (*func)(void*));



// A CwList is a special case of a CwArray where element_size
// is sizeof(void*).
typedef struct CwList { struct CwArray inner; } CwList;

CwList* cwlist_new();
CwList* cwlist_with_elements(size_t size, ...);
void cwlist_push(CwList* self, void* item);
void cwlist_push_many(CwList* self, size_t size, ...);
void* cwlist_get(CwList* self, size_t index);

#endif
