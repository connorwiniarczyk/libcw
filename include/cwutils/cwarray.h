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
#define cwslice_get_as(self, t, i) (*(t*)(cwslice_get(self, i)))

typedef struct CwArray {
    void* ptr;
    int size;
    int element_size;
    CwArena a;
} CwArray;

CwArray cwarray_new(CwArena a, int element_size);
void* cwarray_push(CwArray* self);
void cwarray_clear(CwArray* self);
CwSlice cwarray_finish(CwArray* self, CwArena* a);
int cwarray_size(CwArray* self);

#define cwarray_as(t, self) ((t)(self.ptr))
#define cwarray_as_slice(self) ((CwSlice) { self.ptr, self.size })

typedef struct CwList { struct CwArray inner; } CwList;

CwList cwlist_new();
CwList cwlist_with_elements(CwArena a, size_t size, ...);
void cwlist_push(CwList* self, void* item);
void cwlist_push_many(CwList* self, size_t size, ...);
void* cwlist_get(CwList* self, int index);

#endif
