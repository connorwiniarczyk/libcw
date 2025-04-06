#ifndef CWHOST_H
#define CWHOST_H

#include <stdint.h>
#include <cwcore.h>

void cwhost_exit(int code);
void cwhost_sleepms(int ms);

CwStr cwhost_get_env(const char* key);

int64_t cwhost_get_realtime();
int64_t cwhost_get_monotime();

CwFuture* cwtimeout_ms(CwArena a, int64_t ms);

// -- File IO --
CwStr cwhost_read_file(CwArena* a, const char* path);
int cwhost_write_file(const char* path, CwStr data);
int cwhost_append_file(const char* path, CwStr data);
int cwhost_delete_file(const char* path);
int cwhost_copy_file(const char* src, const char* dest);

typedef struct CwPathList {
    char** ptr;
    int size;
} CwPathList;

CwPathList cwhost_read_dir(CwArena* a, const char* path);
int cwhost_create_dir(const char* path);
int cwhost_delete_dir(const char* path);

typedef struct CwPipe {
	int input;
	int output;
} CwPipe;

CwPipe cwhost_make_pipe();

// -- Command Runner --
typedef struct CwCmd {
    const char** ptr;
    int size;
    CwArena a;

    struct { int in; int out; int err; } io;
} CwCmd;

CwCmd cwcmd_create(const char* cmd, CwArena* a, int max_args);

void cwcmd_push_arg(CwCmd* self, const char* arg);
void cwcmd_push_arglist(CwCmd* self, const char** arglist);

#define cwcmd_push_args(self, ...) do { const char* args[] = {__VA_ARGS__, NULL}; cwcmd_push_arglist(self, args); } while(0);

int cwcmd_run(CwCmd* self);
int cwcmd_spawn(CwCmd* self);
int cwcmd_await(int pid);

// -- Buildtool Helpers --

enum CwBuildTarget {
    CWTARGET_AMD64_POSIX = 0,
    CWTARGET_AMD64_MINGW32,
    CWTARGET_WASM32,
};

typedef struct CwCompileConfig {
    enum CwBuildTarget target;
    const char* src;
    const char* dest;

    struct { CwArena a; char** ptr; } include_paths;
    struct { CwArena a; char** ptr; } flags;
} CwCompileConfig;

CwCompileConfig cwcompile_config(CwArena* a, const char* src, const char* dest);
CwCmd cwcompile_cmd(CwArena* a, CwCompileConfig config);

typedef struct CwLinkConfig {
    enum CwBuildTarget target;
    const char* dest;

    struct { CwArena a; char** ptr; } lib_paths;
    struct { CwArena a; char** ptr; } libs;
    struct { CwArena a; char** ptr; } objects;
    struct { CwArena a; char** ptr; } flags;
} CwLinkConfig;

CwLinkConfig cwlink_config(CwArena* a, const char* dest);
CwCmd cwlink_cmd(CwArena* a, CwLinkConfig config);

CwCmd cwbuild_archive_cmd(CwArena* a, const char* dest_path, const char** objects);

// -- Logging --
typedef struct CwLogger {
    void (*log)(void* data, CwStr message);
    void* data;
    CwArena fmt_buffer;
} CwLogger;

void cwlogger_init(CwArena* a, int buffer_size);
void cwlogger_set(void (*log)(void* data, CwStr message), void* data);

void cwlog(const char* fmt, ...);
void cwlog_error_handler(const char* file, int line, const char* message, ...);

#define cwlog_error(...) cwlog_error_handler(__FILE__, __LINE__, __VA_ARGS__)
#define cwpanic(...) do { cwlog_error_handler(__FILE__, __LINE__, __VA_ARGS__); cwhost_exit(1); } while(0)

// -- Sockets --
int cwhost_cansocket_create();
int cwhost_cansocket_bind(int fd, const char* interface);
int cwhost_cansocket_set_nonblocking(int fd);
int cwhost_cansocket_transmit_ready(int fd);
int cwhost_cansocket_receive_ready(int fd);

#endif
