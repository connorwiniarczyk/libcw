#include <cwplatform.h>
#include <cwcore.h>

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void cwplatform_panic() {
	exit(1);
}

void cwplatform_print(CwStr message) {
    write(1, message.ptr, message.size);
    write(1, "\n", 1);
}

void cwplatform_log(CwStr message) {
    write(2, message.ptr, message.size);
    write(2, "\n", 1);
}

void cwsleep_ms(int ms) {
    usleep(ms * 1000);
}

// static void* timer_thread(void* data) {
//     int* amount = (int*)(data);
//     cwsleep_ms(*amount);
//     *amount = 0;
//     return NULL;
// }

// static int poll_timer(int pc, void* data, CwFuture* self) {
//     int remaining = *((int*)(data));
//     switch (pc) {
//         case 0: return 0;
//         case 1: return remaining == 0 ? 0 : pc;
//         default: return self -> err = 1, 0;
//     }
// }

// CwFuture* cwtimeout_ms(CwArena a, int ms) {
//     pthread_t id;
//     int* remaining = cwalloc(&a, sizeof(int), _Alignof(int), 1);
//     *remaining = ms;

//     int res = pthread_create(&id, NULL, timer_thread, remaining);
//     (void)(res);

//     CwFuture* output = cwfuture_new(a, poll_timer, remaining);
//     return output;
// }
