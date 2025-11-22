#include "unity.h"
#include "utils/utils.h"

void setUp(void) {}

void tearDown(void) {}

void test_add_positive_numbers(void) {
  TEST_ASSERT_EQUAL(8, add(5, 3));
  TEST_ASSERT_EQUAL(15, add(10, 5));
}

void test_add_negative_numbers(void) {
  TEST_ASSERT_EQUAL(-2, add(5, -7));
  TEST_ASSERT_EQUAL(-10, add(-5, -5));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_add_positive_numbers);
  RUN_TEST(test_add_negative_numbers);

  return UNITY_END();
}