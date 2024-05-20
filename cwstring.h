#ifndef CW_STRING
#define CW_STRING

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct CwString {
	uint8_t*  ptr;
	size_t size;
	size_t capacity;
} CwString;

CwString* cw_string_new();
CwString* cw_string_fmt_hex(uint32_t value, int digits);
CwString* cw_string_fmt_int(uint32_t value, int digits);
CwString* cw_string_from_file(char* path);
CwString* cw_string_clone(CwString* self);

void cw_string_free(CwString* self);
int cw_string_push(CwString* self, char c);
int cw_string_push_front(CwString* self, char c);
int cw_string_append_cstr(CwString* self, char* str);
int cw_string_prepend_cstr(CwString* self, char* str);
int cw_string_append_slice(CwString* self, uint8_t* src, size_t size);


#endif

#ifdef CWUTILS_IMPLEMENT_STRING

CwString* cw_string_new() {
    CwString* self = malloc(sizeof(CwString));
    if (self == NULL) return NULL; 

    self -> capacity = 16;
    self -> size = 0;
    self -> ptr = malloc(self -> capacity);
    if (self -> ptr == NULL) return NULL;

    return self;
}

// TODO
// CwString* cw_string_clone(CwString* self) {
// 
// }

CwString* cw_string_from_file(char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) return NULL;

	CwString* self = cw_string_new();
	if (self == NULL) return NULL;

	char buffer[128];
	size_t bytes_read;
	while ((bytes_read = fread(buffer, 1, 128, file)) == 128) {
    	cw_string_append_slice(self, buffer, bytes_read);
	}

	if (ferror(file)) {
    	cw_string_free(self);
    	fclose(file);
    	return NULL;
	}

	cw_string_append_slice(self, buffer, bytes_read);
	return self;
}

CwString* cw_string_fmt_hex(uint32_t value, int digits) {
    CwString* self = cw_string_new();
    for(int i=0; i<digits; i++) {
        uint8_t next = (uint8_t)(value & 0x0F);
        char c = next <= 9 ? next + '0' : (next - 10) + 'A';
        cw_string_push_front(self, c);
        value >>= 4;
    }

    cw_string_push(self, '\0');
    return self;
}

CwString* cw_string_fmt_int(uint32_t value, int digits) {
    CwString* self = cw_string_new();
    for(int i=0; i<digits; i++) {
        uint8_t next = (uint8_t)(value % 10);
        char c = next + '0';
        cw_string_push_front(self, c);
        value = value / 10;
    }

    cw_string_push(self, '\0');
    return self;
}

void cw_string_free(CwString* self) {
  	free(self -> ptr);
  	free(self);
    }

int cw_string_push(CwString* self, char c) {
    if (self -> size >= self -> capacity) {
        do { self -> capacity <<= 1; } while (self -> size >= self -> capacity);
        self -> ptr = realloc(self -> ptr, self -> capacity);
    }

    self -> ptr[self -> size] = c;
    self -> size += 1;
}

int cw_string_push_front(CwString* self, char c) {
    if (self -> size >= self -> capacity) {
        do { self -> capacity <<= 1; } while (self -> size >= self -> capacity);
        self -> ptr = realloc(self -> ptr, self -> capacity);
    }

    memmove(self -> ptr + 1, self -> ptr, self -> size);
    self -> ptr[0] = c;
    self -> size += 1;

    return 0;
}

int cw_string_append_cstr(CwString* self, char* str) {
    while (*str != '\0') cw_string_push(self, *str++);
}

int cw_string_append_slice(CwString* self, uint8_t* src, size_t size) {
    for (int i=0;i<size;i++) cw_string_push(self, src[i]);
}

#endif
