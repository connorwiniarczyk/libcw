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

static int poll_print(int pc, void* data, CwFuture* self) {
    (void)(data); (void)(self); (void)(pc);

    printf("test\n");
	return 0;
}

void test_2() {
    CwList* list = cwlist_with_elements(2, cwtimeout_ms(100), cwfuture_new(poll_print, NULL));
    // cwlist_push(list, cwtimeout_ms(100));
    // cwlist_push(list, cwfuture_new(poll_print, NULL));

    CwFuture* test = cwfuture_sequence(list);
    int err = cwfuture_block_on(test);

    TEST_ASSERT(err == 0);
}

int main() {
    UNITY_BEGIN();
    // RUN_TEST(test_1);
    RUN_TEST(test_2);
    return UNITY_END();
}
