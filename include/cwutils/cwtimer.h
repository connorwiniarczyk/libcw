#ifndef CWTIMER_H
#define CWTIMER_H

#include <cwutils/cwfuture.h>
#include <cwutils/cwsleep.h>

CwFuture* cwtimeout_ms(int ms);
void cwtimeout_init_ms(CwFuture* self, int ms);

#endif
