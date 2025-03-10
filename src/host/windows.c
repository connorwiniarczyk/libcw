#include <cwhost.h>
#include <windows.h>

void cwsleep_ms(int ms) {
    Sleep(ms);
}

void cwhost_exit(int code) {
    exit(code);
}
