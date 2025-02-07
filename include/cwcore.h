#ifndef CWCORE_H
#define CWCORE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// -- Arena Allocator --

typedef struct CwArena {
	uint8_t* start;
	uint8_t* end;
} CwArena;

typedef void* (CwAllocFn)(size_t size);

CwArena cwarena_empty();
CwArena cwarena_create(CwAllocFn* alloc, ptrdiff_t size);
CwArena cwarena_from_buffer(void* buffer, ptrdiff_t size);
CwArena cwarena_reserve(CwArena* self, ptrdiff_t size);

void* cwalloc(CwArena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
#define cwnew(a, t) cwalloc(a, sizeof(t), _Alignof(t), 1)

#define cwarena_local(name, size) uint8_t name ## _mem[size]; \
	CwArena name = { .start = name ## _mem, .end = name ## _mem + size }

// -- Strings -- 

typedef struct CwStr {
	char* ptr;
	int32_t size;
} CwStr;

CwStr cwstr(const char* cstr);

CwStr cwstr_from_file(CwArena* a, const char* path);
CwStr cwstr_substr(CwStr self, int start, int end);
void cwstr_write_to_file(CwStr self, char* path);
bool cwstr_contains(CwStr self, char c);
bool cwstr_equals(CwStr a, CwStr b);

CwStr cwfmt_hex(CwArena* a, int value, int digits);
CwStr cwfmt_dec(CwArena* a, int value, int digits);
CwStr cwfmt_float(CwArena* a, float value, int digits);
CwStr cwfmt(CwArena* a, char* fmt_string, ...);

typedef struct CwString {
	char* ptr;
	int32_t size;
	CwArena mem;
} CwString;

CwString cwstring_new(CwArena a);
void cwstring_push(CwString* self, char c);
void cwstring_push_front(CwString* self, char c);
void cwstring_concat(CwString* self, CwStr* a);
void cwstring_reset(CwString* self);
CwStr cwstring_finish(CwString* self, CwArena* mem);

void cwstring_push_slice(CwString* self, char* src, int size);
void cwstring_push_cstr(CwString* self, char* src);


// -- Dynamic Arrays --

#define cwarray_type(t) struct cwarray_ ## t { t* ptr; int size; CwArena mem; }

#define cwarray_init(arr, type, arena) do { \
    arr.ptr = (void*)(cwalloc(&(arena), sizeof(type), alignof(type), 0));   \
    arr.size = 0;                     \
    arr.mem = arena;                  \
} while(0)

#define cwarray_push(arr, type, value) do {  \
    *(type*)(cwnew(&arr.mem, type)) = value; \
    arr.size += 1;                           \
} while(0)

// -- Command Runner --

typedef struct CwCmd {
    const char** ptr;
    int size;
    CwArena mem;
} CwCmd;

CwCmd cwcmd_create(CwArena a, const char* cmd);
void cwcmd_push_arg(CwCmd* self, const char* arg);
int  cwcmd_run(CwCmd* self);

// -- Ring Buffer --

typedef struct CwRingBuffer {
    struct { void* ptr; int size; } data;
    int front;
    int back;
} CwRingBuffer;

CwRingBuffer cwringbuffer_new(CwArena* a, int element_size, int size);
void* cwringbuffer_push(CwRingBuffer* self);
void* cwringbuffer_next(CwRingBuffer* self);
void* cwringbuffer_peek(CwRingBuffer* self);

#endif
