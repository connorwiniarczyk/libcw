#include <cwutils/cwarena.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <stdio.h>

typedef struct CwStr {
	char* ptr;
	int   size;
} CwStr;

static void push_char(CwArena* a, char c) {
    char* next = cwalloc(a, 1, 1, 1);
	*next = c;
}

static void push_char_front(char* front, CwArena* a, char c) {
    char* next = cwalloc(a, 1, 1, 1);

    // move every character up by 1 byte
    for (; next > front; next--) *next = *(next - 1);

    *front = c;
}

CwStr cwstr_read_file(CwArena* a, char* path) {
    CwStr output = { (char*)(a -> start), 0 };
    FILE* file = fopen(path, "rb");
    if (file == NULL) return (CwStr){ NULL, 0 };

	int bytes_read;
	while ((bytes_read = fread(cwalloc(a, 1, 1, 1), 1, 1, file)) == 1);

	return output;

}


CwStr cwfmt_hex(CwArena* a, int value, int digits) {
    char* output = (char*)(a -> start);

	int i;
    for (i=0; digits == 0 ? value > 0 : i<digits; i++) {
        uint8_t digit = (uint8_t)(value & 0x0F);
        push_char_front(output, a, digit <= 9 ? digit + '0' : (digit - 10) + 'A');
        value >>= 4;
    }

    if (i & 1) push_char_front(output, a, '0'); 

    return (CwStr){ output, digits };
}

CwStr cwfmt_dec(CwArena* a, int value, int digits) {
    char* output = (char*)(a -> start);

	int i;
    for (i=0; digits == 0 ? value > 0 : i<digits; i++) {
        uint8_t digit = (uint8_t)(value % 10);
        push_char_front(output, a, digit + '0');
        // push_char_front(output, a, digit <= 9 ? digit + '0' : (digit - 10) + 'A');
        value /= 10;
    }

    if (i == 0) push_char(a, '0');

    return (CwStr){ output, digits };
}

CwStr cwfmt(CwArena* a, char* fmt_string, ...) {
    char* output = (char*)(a -> start);

    va_list args;
    va_start(args, fmt_string);

    int i;
    for (i=0; fmt_string[i] != '\0'; i++) {
        char next = fmt_string[i];
        if (next != '%') push_char(a, next);
        else {
           i += 1;
           switch (fmt_string[i]) {
               case 'x': (void)cwfmt_hex(a, va_arg(args, int), 0);  break;
               case 'd': (void)cwfmt_dec(a, va_arg(args, int), 0);  break;
               case 'c': push_char(a, va_arg(args, int)); break;
               case 's': {
                   char* str = va_arg(args, char*);
                   for (int i=0; str[i] != '\0'; i++) push_char(a, str[i]);
                   break;
               }
               default: break;
           }
        }
    }

    push_char(a, '\0');
    va_end(args);
    return (CwStr){ output, i };
}
