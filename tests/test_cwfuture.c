#include <unity.h>
#include <cwfuture.h>

void setUp() {}
void tearDown() {}

void test_1() {
    TEST_ASSERT(true);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_1);
    return UNITY_END();
}
