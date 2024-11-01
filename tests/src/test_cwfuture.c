#include <unity.h>
#include <cwutils/cwfuture.h>
#include <cwutils/cwtimer.h>
#include <cwutils/cwarena.h>

void setUp() {}
void tearDown() {}

void test_1() {
    CwArena a = cwarena_new(0x200);
    CwFuture* test = cwtimeout_ms(a, 100);
    int err = cwfuture_run(test);
    TEST_ASSERT(err == 0);
}

void test_loop() {
	int poll(int pc, void* data, CwFuture* self) {
        switch (pc) {
			case 1: return cwfuture_await(self, cwtimeout_ms(self -> arena, 100));
			case 2: return fprintf(stderr, "counter: %d\n", *(int*)(data)), pc + 1; 
			case 3: return (*(int*)(data))++ == 10 ? 0 : 1;
			default: return pc;
        }
	}

	CwArena a = cwarena_new(0x100);
	int* counter = cwnew(&a, int);
	int err = cwfuture_run(cwfuture_new(a, &poll, counter));

	TEST_ASSERT_EQUAL(0, err);
}

int on_arena_overflow(CwArena* data) {
    (void)(data);
    TEST_FAIL_MESSAGE("Arena Overflowed");
}

void test_race() {
	CwArena a = cwarena_new(0x400);
	a.on_overflow = on_arena_overflow;
	CwList* list = cwlist_new();

	for (int i=0; i<10; i++) {
    	CwFuture* timeout = cwtimeout_ms(cwarena_section(&a, sizeof(CwFuture) + sizeof(int) + 0x04), 100 * (i + 1));
    	cwlist_push(list, timeout);
	}

	int index = cwfuture_run(cwfuture_race(a, list));
	TEST_ASSERT_EQUAL(0, index);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_1);
    RUN_TEST(test_loop);
    RUN_TEST(test_race);
    return UNITY_END();
}
