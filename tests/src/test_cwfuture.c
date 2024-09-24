#include <unity.h>
#include <cwutils/cwfuture.h>
#include <cwutils/cwtimer.h>

void setUp() {}
void tearDown() {}

void test_1() {
    CwFuture* test = cwtimeout_ms(100);
    int err = cwfuture_block_on(test);
    TEST_ASSERT(err == 0);
}

void test_2() {
    CwFutureList* list = cwfuture_list_new();
    cwfuture_list_push(list, cwtimeout_ms(100));
    cwfuture_list_push(list, cwtimeout_ms(200));
    cwfuture_list_push(list, cwtimeout_ms(300));
    cwfuture_list_push(list, cwtimeout_ms(400));
    cwfuture_list_push(list, cwtimeout_ms(500));
    cwfuture_block_on(cwfuture_race(list));

    CwFuture* test = cwtimeout_ms(100);
    int err = cwfuture_block_on(test);
    TEST_ASSERT(err == 0);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_1);
    RUN_TEST(test_2);
    return UNITY_END();
}
