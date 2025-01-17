#ifndef CWARENA_H
#define CWARENA_H

#include <stdint.h>
#include <stddef.h>

typedef struct CwArena {
	uint8_t* start;
	uint8_t* end;
	int (*on_overflow)(struct CwArena*);
} CwArena;

void* cwalloc(CwArena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
CwArena cwarena_new(ptrdiff_t size);
CwArena cwarena_section(CwArena* self, ptrdiff_t size);

CwArena* cwarena_scratch(CwArena* self);

#define cwnew(a, t) cwalloc(a, sizeof(t), _Alignof(t), 1)

#endif
