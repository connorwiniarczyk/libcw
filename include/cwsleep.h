#ifndef CWSLEEP_H
#define CWSLEEP_H

#if defined(__linux__)
#include <unistd.h>

#elif defined(__WINDOWS__)
#include <windows.h>

#else
#error "cwsleep is not supported for this os"

#endif

void cwsleep_ms(int ms);

#endif

#ifdef CWSLEEP_IMPLEMENTATION

    #ifdef __linux__
    void cwsleep_ms(int ms) { usleep(ms * 1000); }

    #elif defined(__WINDOWS__)
    void cwsleep_ms(int ms) { Sleep(ms); }

    #endif

#endif
