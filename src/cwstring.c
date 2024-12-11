#include <cwutils/cwstring.h>

CwString cwstring_new(CwArena a) {
    CwString output = {
		.ptr = (char*)a.start,
		.size = 0,
		.a = a,
    };

    return output;
}

void cwstring_push(CwString* self, char c) {
    char* next = cwalloc(&self -> a, 1, 1, 1);
    *next = c;
}

void cwstring_push_front(CwString* self, char c) {
    char* next = cwalloc(&self -> a, 1, 1, 1);

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

CwStr cwstr_from_file(CwArena* a, char* path) {
    CwStr output = {0};
    FILE* file = fopen(path, "rb");
    if (file == NULL) return output;

    output.ptr = (char*)(a -> start);

	size_t bytes_read;
	while ((bytes_read = fread(cwalloc(a, 1, 1, 1), 1, 1, file)) == 1) output.size++;

	fclose(file);

	return output;
}

CwStr cwstr_substr(CwStr* self, int start, int end) {
    return (CwStr){ self -> ptr + start, end - start };
}

CwStr cwstring_finish(CwString* self, CwArena* a) {
	*a = self -> a;
	return (CwStr){ .ptr = self -> ptr, .size = self -> size };
}

void cwstring_clear(CwString* self) {
    self -> a.start = (void*)self -> ptr;
}




// int cwstring_write_to_file(CwString* self, char* path) {
//     FILE* file = fopen(path, "wb");
//     if (file == NULL) return 1;

//     size_t bytes_written = fwrite(self -> ptr, 1, self -> size, file);
//     if (bytes_written != self -> size) return 2;

//     return 0;
// }
