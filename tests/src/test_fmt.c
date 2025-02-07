#include <cwcore.h>
#include <cwplatform.h>

#include <unity.h>
#include <math.h>

void setUp() {}
void tearDown() {}

void test_01() {
    uint8_t local_mem[1 << 9] = { 0x00 };
	CwArena local = { .start = local_mem, .end = local_mem + (1 << 9) }; 
	CwStr test = cwfmt_float(&local, 12.34, 2);
    TEST_ASSERT_EQUAL_STRING_LEN("12.34", test.ptr, 5);
}

void test_02() {
    uint8_t local_mem[1 << 9] = { 0xEE };
	CwArena local = { .start = local_mem, .end = local_mem + (1 << 9) }; 
	CwStr test = cwfmt(&local, "%2f", 12.34);
    TEST_ASSERT_EQUAL_STRING_LEN("12.34", test.ptr, 5);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01);
    RUN_TEST(test_02);
    return UNITY_END();
}
