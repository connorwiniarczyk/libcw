#include <cwcore.h>
#include <cwhost.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdalign.h>

#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

CwStr cwhost_read_file(CwArena* a, const char* path) {
	CwStr output;
	output.ptr = (char*)(a -> start);

	int fd = open(path, 0);
	if (fd < 0) return cwstr_empty();

	int max_size = cwarena_remaining(*a);
	output.size = read(fd, output.ptr, max_size);
	a -> start += output.size;

	close(fd);
	return output;
}

int cwhost_write_file(const char* path, CwStr self) {
    FILE* file = fopen(path, "wb");
    if (file == NULL) return 1;

    int bytes_written = fwrite(self.ptr, 1, self.size, file);
    if (bytes_written != self.size) return 2;

    return 0;
}

int cwhost_append_file(const char* path, CwStr self) {
    FILE* file = fopen(path, "ab");
    if (file == NULL) return 1;

    int bytes_written = fwrite(self.ptr, 1, self.size, file);
    if (bytes_written != self.size) return 2;

    return 0;
}

int cwhost_create_dir(const char* input) {
    int err;
    uint8_t fmt_buffer[1 << 8];

    CwStr path = cwstr(input);
    CwStr subpath;
    subpath.ptr = path.ptr;
    subpath.size = 0;

    for (int i=0; i<path.size; i++, subpath.size ++) {
        if (path.ptr[i] == '/' && i != 0) {
            CwArena a = cwarena_from_buffer(fmt_buffer, 1 << 8);
            char* cstr = cwfmt_cstr(&a, "%w", subpath);
            err = mkdir(cstr, 0755);
            if (err && errno != EEXIST) return errno;
        }
    }

    err = mkdir(input, 0755);
    if (err && errno != EEXIST) return errno;

    return 0;
}

int cwhost_delete_file(const char* path) {
	int err = remove(path);
	if (err == -1) return errno;

	else return 0;
}

int cwhost_delete_dir(const char* path) {
	int err = rmdir(path);
	if (err == -1) return errno;

	else return 0;
}

int cwhost_copy_file(const char* src_path, const char* dest_path) {
	int src = open(src_path, O_RDONLY);
	if (src < 0) return errno;

	int dest = open(dest_path, O_CREAT | O_WRONLY, 0664);
	if (dest < 0) return errno;

	int n;
	do { n = sendfile(dest, src, 0, 1 << 14); } while(n > 0);

	if (n == -1) return errno;

	return 0;
}

CwPathList cwhost_read_dir(CwArena* a, const char* path) {
    CwPathList output = {0};
    char* ptr = (char*)(a -> start);

    DIR* dir = opendir(path);
    if (dir == NULL) return output; 

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry -> d_name, ".")  == 0) continue;
        if (strcmp(entry -> d_name, "..") == 0) continue;

		(void)cwfmt(a, "%s/%s", path, entry -> d_name);
		output.size += 1;
    }

	output.ptr = cwalloc(a, sizeof(char*), alignof(char*), output.size);

	for (int i=0; i<output.size; i++) {
    	output.ptr[i] = ptr;
    	while(ptr[0] != '\0') ptr += 1;

    	ptr += 1;
	}

    return output;
}
