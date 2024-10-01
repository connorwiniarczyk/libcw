#include <cwutils/cwtimer.h> 

static void* timer_thread(void* data) {
    int* amount = (int*)(data);
    cwsleep_ms(*amount);
    *amount = 0;
    return NULL;
}

static int poll_timer(int pc, void* data, CwFuture* self) {
    (void)(self);

    int remaining = *((int*)(data));
    switch (pc) {
        case 1: return remaining == 0 ? pc + 1 : pc;
        case 2: return free(data), 0;
        default: return free(data), -1;
    }
}

#if defined(LINUX)
#include <pthread.h>
CwFuture* cwtimeout_ms(int ms) {
    pthread_t id;
    int* remaining = malloc(sizeof(int));
    *remaining = ms;
    int res = pthread_create(&id, NULL, timer_thread, remaining);
    (void)(res);

    CwFuture* output = cwfuture_new(poll_timer, remaining);
    cwfuture_on_cleanup(output, free, remaining);
    return output;
}

void cwtimeout_init_ms(CwFuture* self, int ms) {
    pthread_t id;
    int* remaining = malloc(sizeof(int));
    *remaining = ms;
    int res = pthread_create(&id, NULL, timer_thread, remaining);
    (void)(res);

    cwfuture_init(self, poll_timer, remaining);
    cwfuture_on_cleanup(self, free, remaining);
}

#else
#error 

#endif
