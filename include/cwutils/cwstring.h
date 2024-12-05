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
CwStr cwstr_substr(CwStr* self, int start, int end);

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

#endif
