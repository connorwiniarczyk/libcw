#ifndef CWCORE_H
#define CWCORE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

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

// -- Pool Allocator --
typedef struct CwPool {
    void* next_free;
    size_t element_size;
} CwPool;

CwPool cwpool_create(CwArena* a, size_t element_size, int size);

void* cwpool_alloc(CwPool* self);
void  cwpool_free(CwPool* self, void* ptr);

// -- Strings -- 

typedef struct CwStr {
	char* ptr;
	int32_t size;
} CwStr;

CwStr cwstr(const char* cstr);
CwStr cwstr_empty();

CwStr cwstr_from_file(CwArena* a, const char* path);
CwStr cwstr_substr(CwStr self, int start, int end);
void cwstr_write_to_file(CwStr self, char* path);
bool cwstr_contains(CwStr self, char c);
bool cwstr_equals(CwStr a, CwStr b);

CwStr cwfmt_hex(CwArena* a, int value, int digits);
CwStr cwfmt_dec(CwArena* a, int value, int digits);
CwStr cwfmt_float(CwArena* a, float value, int digits);
CwStr cwfmt(CwArena* a, const char* fmt_string, ...);
CwStr cwfmtV(CwArena* a, const char* fmt_string, va_list args);

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

// -- Futures ---
struct CwFuture;
typedef int (PollFn)(int pc, void* data, struct CwFuture* self);
typedef struct CwFuture {
    int pc; // program counter
    int err;

    PollFn* poll;
    void*   data;
    CwArena arena;

    struct CwFuture* child;
} CwFuture;

CwFuture* cwfuture_new(CwArena a, PollFn* poll, void* data);

int cwfuture_poll(CwFuture* self);
int cwfuture_run(CwFuture* self);
int cwfuture_abort(CwFuture* self, int err);
int cwfuture_await(CwFuture* self, CwFuture* target);
int cwfuture_await_new(CwFuture* self, PollFn* poll, void* data);

#endif
