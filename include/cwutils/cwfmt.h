#include <cwutils/cwarena.h>

typedef struct CwStr {
	char* ptr;
	int size;
} CwStr;

CwStr cwfmt_hex(CwArena* a, int value, int digits);
CwStr cwfmt_dec(CwArena* a, int value, int digits);
CwStr cwfmt(CwArena* a, char* fmt_string, ...);
