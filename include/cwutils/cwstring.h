#ifndef CWSTRING_H
#define CWSTRING_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cwutils/cwarena.h>

typedef struct CwStr {
	char* ptr;
	int32_t size;
} CwStr;

CwStr cwstr_from_file(CwArena* a, char* path);
void cwstr_write_to_file(CwStr* self, char* path);

typedef struct CwString {
	char* ptr;
	int32_t size;
	CwArena a;
} CwString;

CwString cwstring_new(CwArena a);
void cwstring_push(CwString* self, char c);
void cwstring_push_front(CwString* self, char c);
void cwstring_concat(CwString* self, CwStr* a);
void cwstring_reset(CwString* self);
CwStr cwstring_finish(CwString* self, CwArena* mem);

void cwstring_push_slice(CwString* self, char* src, int size);
void cwstring_push_cstr(CwString* self, char* src);

// void cwstring_free(CwString* self);
// int cwstring_push(CwString* self, char c);
// int cwstring_push_front(CwString* self, char c);
// int cwstring_append_cstr(CwString* self, const char* str);
// int cwstring_prepend_cstr(CwString* self, char* str);
// int cwstring_append_slice(CwString* self, uint8_t* src, size_t size);
// int cwstring_clear(CwString* self);
// void cwstring_add_terminator(CwString* self);

// int cwstring_write_to_file(CwString* self, char* path);

#endif
