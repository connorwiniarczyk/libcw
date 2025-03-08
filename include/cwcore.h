#ifndef CWCORE_H
#define CWCORE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdalign.h>

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
CwArena cwarena_reserve_all(CwArena* self);

void* cwarena_align_to(CwArena* self, ptrdiff_t align);

void* cwarena_push_byte(CwArena* self, uint8_t byte);
void* cwarena_push_ptr(CwArena* self, const void* ptr);

void* cwarena_push_list(CwArena* self, const void** data);
#define cwarena_push_ptrs(a, ...) do { const void* list[] = { __VA_ARGS__, NULL }; cwarena_push_list(a, list); } while(0);

int cwarena_allocated(CwArena self, void* start);
int cwarena_remaining(CwArena self);

void* cwalloc(CwArena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count);
#define cwnew(a, t) cwalloc(a, sizeof(t), alignof(t), 1)

#define cwarena_push(a, t, value) do { *((t*)(cwalloc(a, sizeof(t), alignof(t), 1))) = value; } while(0)

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
CwStr cwstr_builder(CwArena* a);

int cwstr_find(CwStr input, char c);
int cwstr_find_last(CwStr input, char c);

int cwstr_parse_int(CwStr input);

CwStr cwstr_substr(CwStr self, int start, int end);
CwStr cwstr_split(CwStr* self, char c);

bool cwstr_contains(CwStr self, char c);
bool cwstr_equals(CwStr a, CwStr b);

CwStr cwpath_get_file(CwStr input);
CwStr cwpath_get_dir(CwStr input);
CwStr cwpath_get_base(CwStr input);
CwStr cwpath_get_ext(CwStr input);

CwStr cwfmt_hex(CwArena* a, int value, int digits);
CwStr cwfmt_dec(CwArena* a, int value, int digits);
CwStr cwfmt_float(CwArena* a, float value, int digits);

CwStr cwfmt_vargs(CwArena* a, const char* fmt_string, va_list args);
CwStr cwfmt(CwArena* a, const char* fmt_string, ...);

char* cwfmt_cstr(CwArena* a, const char* fmt_string, ...);

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
