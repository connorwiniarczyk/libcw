#ifndef CWTIMER_H
#define CWTIMER_H

#include <cwutils/cwfuture.h>
#include <cwutils/cwsleep.h>

CwFuture* cwtimeout_ms(int ms);
void cwtimeout_init_ms(CwFuture* self, int ms);

// struct CwTimer;
// CwTimer* cwtimer_new();
// void cwtimer_start(CwTimer* self);
// void cwtimer_stop(CwTimer* self);
// int cwtimer_elapsed(CwTimer* self);

#endif
