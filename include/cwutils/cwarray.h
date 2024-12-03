#ifndef CWARRAY_H
#define CWARRAY_H

#include <stdint.h>
#include <cwutils/cwarena.h>

typedef struct CwSlice {
    void* ptr;
    int size;
    int element_size;
} CwSlice;

void* cwslice_get(CwSlice* self, int index);

typedef struct CwArray {
    void* ptr;
    int size;
    int element_size;
    CwArena a;
} CwArray;

CwArray cwarray_new(CwArena a, int element_size);
void* cwarray_push(CwArray* self);

void cwarray_clear(CwArray* self);
CwSlice cwarray_finish(CwArray self, CwArena* a);

int cwarray_size(CwArray* self);

// typedef struct CwArray {
// 	void* ptr;
// 	int element_size;
// 	int size;
// 	int capacity;
// } CwArray;

// CwArray* cwarray_new(size_t element_size);
// void*    cwarray_push(CwArray* self);
// void*    cwarray_get(CwArray* self, size_t index);
// void*    cwarray_demote(CwArray* self);

// int cwarray_size(CwArray* self);
// CwArray* cwarray_free(CwArray* self);
// void cwarray_for_each(CwArray* self, void (*func)(void*));

// A CwList is a special case of a CwArray where element_size
// is sizeof(void*).
typedef struct CwList { struct CwArray inner; } CwList;

CwList cwlist_new();
CwList cwlist_with_elements(CwArena a, size_t size, ...);
void cwlist_push(CwList* self, void* item);
void cwlist_push_many(CwList* self, size_t size, ...);
void* cwlist_get(CwList* self, int index);

#endif
