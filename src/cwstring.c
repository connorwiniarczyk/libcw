#include <cwcore.h>
#include <assert.h>
#include <string.h>

CwStr cwstr(const char* cstr) {
    return (CwStr){ .ptr = (char*)(cstr), .size = strlen(cstr) };
}

CwStr cwstr_empty() {
    return (CwStr){ .ptr = NULL, .size = 0 };
}

bool cwstr_equals(CwStr a, CwStr b) {
    if (a.size != b.size) return false;

    for (int i=0; i<a.size; i++) {
		if (a.ptr[i] != b.ptr[i]) return false;
    }

    return true;
}



bool cwstr_contains(CwStr self, char c) {
    for (int i=0; i<self.size; i++) {
		if (self.ptr[i] == c) return true;
    }

    return false;
}

CwStr cwstring_finish(CwString* self, CwArena* a) {
	*a = self -> mem;
	self -> mem = cwarena_empty();
	return (CwStr){ .ptr = self -> ptr, .size = self -> size };
}

void cwstring_clear(CwString* self) {
    self -> mem.start = (void*)self -> ptr;
}

// int cwstring_write_to_file(CwString* self, char* path) {
//     FILE* file = fopen(path, "wb");
//     if (file == NULL) return 1;

//     size_t bytes_written = fwrite(self -> ptr, 1, self -> size, file);
//     if (bytes_written != self -> size) return 2;

//     return 0;
// }


CwString cwstring_new(CwArena a) {
    CwString output = {
		.ptr = (char*)a.start,
		.size = 0,
		.mem = a,
    };

    return output;
}

CwStr cwstr_substr(CwStr self, int start, int end) {
    assert(start >= 0);
    assert(end >= start);
    assert(end <= self.size);

    return (CwStr) { self.ptr + start, end - start };

}

void cwstring_push(CwString* self, char c) {
    char* next = cwalloc(&self -> mem, 1, 1, 1);
    *next = c;
}

void cwstring_push_front(CwString* self, char c) {
    char* next = cwalloc(&self -> mem, 1, 1, 1);

    // move every character up by one
    for (; next > self -> ptr; next--) *next = *(next - 1);

    *next = c;
}

void cwstring_push_slice(CwString* self, char* src, int size) {
    for (int i=0; i<size; i++) cwstring_push(self, src[i]);
}

void cwstring_push_cstr(CwString* self, char* src) {
    for (int i=0; src[i] != '\0'; i++) cwstring_push(self, src[i]);
}
