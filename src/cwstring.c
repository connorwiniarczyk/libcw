#include <cwutils/cwstring.h>

CwString* cwstring_new() {
    CwString* self = malloc(sizeof(CwString));
    if (self == NULL) return NULL; 

    self -> capacity = 16;
    self -> size = 0;
    self -> ptr = malloc(self -> capacity);
    if (self -> ptr == NULL) return NULL;

    for (size_t i=0;i<self -> capacity;i++) self -> ptr[i] = '\0';

    return self;
}

CwString* cwstring_from_file(char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) return NULL;

	CwString* self = cwstring_new();
	if (self == NULL) return NULL;

	uint8_t buffer[128];
	size_t bytes_read;
	while ((bytes_read = fread(buffer, 1, 128, file)) == 128) {
    	cwstring_append_slice(self, buffer, bytes_read);
	}

	if (ferror(file)) {
    	cwstring_free(self);
    	fclose(file);
    	return NULL;
	}

	cwstring_append_slice(self, buffer, bytes_read);
	return self;
}

int cwstring_write_to_file(CwString* self, char* path) {
    FILE* file = fopen(path, "wb");
    if (file == NULL) return 1;

    size_t bytes_written = fwrite(self -> ptr, 1, self -> size, file);
    if (bytes_written != self -> size) return 2;

    return 0;
}

CwString* cwstring_fmt_hex(uint32_t value, int digits) {
    CwString* self = cwstring_new();
    for(int i=0; i<digits; i++) {
        uint8_t next = (uint8_t)(value & 0x0F);
        char c = next <= 9 ? next + '0' : (next - 10) + 'A';
        cwstring_push_front(self, c);
        value >>= 4;
    }

    cwstring_push(self, '\0');
    return self;
}

CwString* cwstring_fmt_int(uint32_t value, int digits) {
    CwString* self = cwstring_new();
    for(int i=0; i<digits; i++) {
        uint8_t next = (uint8_t)(value % 10);
        char c = next + '0';
        cwstring_push_front(self, c);
        value = value / 10;
    }

    cwstring_push(self, '\0');
    return self;
}

void cwstring_free(CwString* self) {
  	free(self -> ptr);
  	free(self);
}

int cwstring_push(CwString* self, char c) {
    if (self -> size >= self -> capacity) {
        do { self -> capacity <<= 1; } while (self -> size >= self -> capacity);
        self -> ptr = realloc(self -> ptr, self -> capacity);
    }

    self -> ptr[self -> size] = c;
    self -> size += 1;
    return 0;
}

int cwstring_push_front(CwString* self, char c) {
    if (self -> size >= self -> capacity) {
        do { self -> capacity <<= 1; } while (self -> size >= self -> capacity);
        self -> ptr = realloc(self -> ptr, self -> capacity);
    }

    memmove(self -> ptr + 1, self -> ptr, self -> size);
    self -> ptr[0] = c;
    self -> size += 1;

    return 0;
}

int cwstring_append_cstr(CwString* self, const char* str) {
    while (*str != '\0') cwstring_push(self, *str++);
    return 0;
}

int cwstring_append_slice(CwString* self, uint8_t* src, size_t size) {
    for (size_t i=0;i<size;i++) cwstring_push(self, (char)src[i]);
    return 0;
}

int cwstring_clear(CwString* self) {
    self -> size = 0;
    return 0;
}

void cwstring_add_terminator(CwString* self) {
	if (self -> ptr[self -> size] != '\0') {
		cwstring_push(self, '\0');
	}
}
