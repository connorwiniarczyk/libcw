#include <unity.h>
#include <cwutils/cwfuture.h>
#include <cwutils/cwtimer.h>
#include <cwutils/cwarena.h>

void setUp() {}
void tearDown() {}

void test_1() {
    CwArena a = cwarena_new(0x200);
    CwFuture* test = cwtimeout_ms(100);
    int err = cwfuture_run(test);
    TEST_ASSERT(err == 0);
}

void test_loop() {
	int poll(int pc, void* data, CwFuture* self) {
        int* counter = (int*)(data);
        switch (pc) {
			case 1: return cwfuture_await(self, cwtimeout_ms(self -> arena, 100));
			case 2: return fprintf(stderr, "counter: %d\n", *counter), pc + 1; 
			case 3: return (*counter)++ == 10 ? 0 : 1;
			default: return pc;
	}
}

void test_loop(void) {
    int poll(int pc, void* data, CwFuture* self) {
        int* counter = (int*)(data);
        switch (pc) {
			case 1: return cwfuture_await(self, cwtimeout_ms(100));
			case 2: return fprintf(stderr, "counter: %d\n", *counter), pc + 1; 
			case 3: return (*counter)++ == 10 ? 0 : 1;

			default: return pc;
        }
    }

	int counter = 0;
    cwfuture_block_on(cwfuture_new(poll, &counter));
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
    RUN_TEST(test_loop);
    return UNITY_END();
}
