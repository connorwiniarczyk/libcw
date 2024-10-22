#ifndef CWARENA_H
#define CWARENA_H

#include <stdint.h>
#include <stddef.h>

typedef struct CwArena {
	uint8_t* start;
	uint8_t* end;
} CwArena;

void* cwalloc(CwArena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
CwArena cwarena_new(ptrdiff_t size);

#endif
