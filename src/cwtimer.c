#include <cwutils/cwtimer.h> 
#include <cwutils/cwarena.h> 

static void* timer_thread(void* data) {
    int* amount = (int*)(data);
    cwsleep_ms(*amount);
    *amount = 0;
    return NULL;
}

static int poll_timer(int pc, void* data, CwFuture* self) {
    int remaining = *((int*)(data));
    // printf("%d\n", remaining);
    switch (pc) {
        case 1: return remaining == 0 ? 0 : pc;
        default: return self -> err = 1, 0;
    }
}

#if defined(LINUX)
#include <pthread.h>
CwFuture* cwtimeout_ms(CwArena a, int ms) {
    pthread_t id;
    int* remaining = cwalloc(&a, sizeof(int), _Alignof(int), 1);
    *remaining = ms;

    int res = pthread_create(&id, NULL, timer_thread, remaining);
    (void)(res);

    CwFuture* output = cwfuture_new(a, poll_timer, remaining);
    return output;
}

#else
#error 

#endif
