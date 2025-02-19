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
    CwArena mem;

    struct { int in; int out; int err; } io;
} CwCmd;

CwCmd cwcmd_create(CwArena a, const char* cmd);
void cwcmd_push_arg(CwCmd* self, const char* arg);
void cwcmd_push_arglist(CwCmd* self, const char** arglist);

#define cwcmd_push_args(self, ...) do { const char* args[] = {__VA_ARGS__, NULL}; cwcmd_push_arglist(self, args); } while(0);

int cwcmd_run(CwCmd* self);

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

#endif
