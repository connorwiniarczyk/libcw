#include <cwutils/cwarena.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

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

    if (i == 0) {
        push_char_front(output, a, '0'); 
        i += 1;
    }

    if (i & 1) push_char_front(output, a, '0'); 

    return (CwStr){ output, digits };
}

CwStr cwfmt_dec(CwArena* a, int value, int digits) {
    char* output = (char*)(a -> start);

    bool is_negative = value < 0;

    if (is_negative) value *= -1;

	int i;
    for (i=0; digits == 0 ? value > 0 : i<digits; i++) {
        uint8_t digit = (uint8_t)(value % 10);
        push_char_front(output, a, digit + '0');
        value /= 10;
    }

    if (i == 0) push_char(a, '0');

    if (is_negative) push_char_front(output, a, '-');

	ptrdiff_t size = (intptr_t)(a -> start) - (intptr_t)(output);
    return (CwStr){ output, size };
}

CwStr cwfmt_float(CwArena* a, float value, int precision) {
    char* output = (char*)(a -> start);

    if (value < 0.0) {
        push_char(a, '-');
        value *= -1;
    }

    int whole = (int)(floor(value));
    (void)(cwfmt_dec(a, whole, 0));

    push_char(a, '.');
	float fraction = value - (float)(whole);

	int i;
    for (i=0; i<precision; i++) {
        fraction *= 10;
        int digit = (int)(floor(fraction));
        fraction -= digit;
        push_char(a, digit + '0');
    }

    if (i == 0) push_char(a, '0');

	ptrdiff_t size = (intptr_t)(a -> start) - (intptr_t)(output);
    return (CwStr){ output, size };
}

bool is_digit(char c) { return c >= '0' && c <= '9'; };

CwStr cwfmtV(CwArena* a, const char* fmt_string, va_list args) {
    char* output = (char*)(a -> start);

    // va_list args;
    // va_start(args, fmt_string);

    int i;
    for (i=0; fmt_string[i] != '\0'; i++) {
        char next = fmt_string[i];
        if (next != '%') push_char(a, next);
        else {
            int precision = 0;
            i += 1;
            if (is_digit(fmt_string[i])) {
				precision = fmt_string[i] - '0';
				i += 1;
            }

            switch (fmt_string[i]) {
               case 'x': (void)cwfmt_hex(a, va_arg(args, int), precision);  break;
               case 'd': (void)cwfmt_dec(a, va_arg(args, int), precision);  break;
               case 'f': (void)cwfmt_float(a, va_arg(args, double), precision);  break;
               case 'c': push_char(a, va_arg(args, int)); break;
               case 'w': {
					CwStr str = va_arg(args, CwStr);
					for (int i=0;i<str.size; i++) push_char(a, str.ptr[i]);
					break;
               }
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
    // va_end(args);

	ptrdiff_t size = (intptr_t)(a -> start) - (intptr_t)(output);
    return (CwStr){ output, size };

}

CwStr cwfmt(CwArena* a, const char* fmt_string, ...) {
    va_list args;
    va_start(args, fmt_string);
    CwStr output = cwfmtV(a, fmt_string, args);
    va_end(args);

    return output;
 //    char* output = (char*)(a -> start);

 //    va_list args;
 //    va_start(args, fmt_string);

 //    int i;
 //    for (i=0; fmt_string[i] != '\0'; i++) {
 //        char next = fmt_string[i];
 //        if (next != '%') push_char(a, next);
 //        else {
 //            int precision = 0;
 //            i += 1;
 //            if (is_digit(fmt_string[i])) {
	// 			precision = fmt_string[i] - '0';
	// 			i += 1;
 //            }

 //            switch (fmt_string[i]) {
 //               case 'x': (void)cwfmt_hex(a, va_arg(args, int), precision);  break;
 //               case 'd': (void)cwfmt_dec(a, va_arg(args, int), precision);  break;
 //               case 'f': (void)cwfmt_float(a, va_arg(args, double), precision);  break;
 //               case 'c': push_char(a, va_arg(args, int)); break;
 //               case 's': {
 //                   char* str = va_arg(args, char*);
 //                   for (int i=0; str[i] != '\0'; i++) push_char(a, str[i]);
 //                   break;
 //               }
 //               default: break;
 //            }
 //        }
 //    }

 //    push_char(a, '\0');
 //    va_end(args);

	// ptrdiff_t size = (intptr_t)(a -> start) - (intptr_t)(output);
 //    return (CwStr){ output, size };
}
