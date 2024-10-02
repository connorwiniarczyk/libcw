#include <cwutils/cwfuture.h>
#include <cwutils/cwtimer.h>

void setUp() {}
void tearDown() {}

int main() {
    CwFuture* test = cwtimeout_ms(200);
    cwfuture_free(test);
    // cwfuture_block_on(test);

    return 0;
}
