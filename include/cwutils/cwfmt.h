#ifndef CWFMT_H
#define CWFMT_H

#include <cwutils/cwarena.h>
#include <cwutils/cwstring.h>

CwStr cwfmt_hex(CwArena* a, int value, int digits);
CwStr cwfmt_dec(CwArena* a, int value, int digits);
CwStr cwfmt(CwArena* a, char* fmt_string, ...);

#endif
