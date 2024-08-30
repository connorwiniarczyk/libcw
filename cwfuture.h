#ifndef CWFUTURE_H
#define CWFUTURE_H

struct CwRuntime;

typedef struct CwFuture {
    int (*poll)(struct CwFuture* self);
    int pc;
    void* data;
    struct CwRuntime* runtime;
} CwFuture;

typedef struct CwRuntime {

} CwRuntime;

int cwfuture_poll(CwFuture* self);
int cwfuture_await(CwFuture* self, CwFuture* value);

#endif

#ifdef CWFUTURE_IMPLEMENTATION

int cwfuture_poll(CwFuture* self) {
    int res = self -> poll(self);
    self -> pc = res;
}

#endif
