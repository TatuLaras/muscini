#include "common.h"
#include "unity.h"
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

void test_max_min(void) {
    uint64_t smaller = 5;
    uint64_t larger = 8;
    TEST_ASSERT_EQUAL(larger, max(smaller, larger));
    TEST_ASSERT_EQUAL(smaller, min(smaller, larger));

    float smaller_f = 5.1;
    float larger_f = 5.2;
    TEST_ASSERT_EQUAL(larger_f, max(smaller_f, larger_f));
    TEST_ASSERT_EQUAL(smaller_f, min(smaller_f, larger_f));
}

void test_quantize(void) {
    TEST_ASSERT_EQUAL_FLOAT(4.5, quantize(4.3, 0.5));
    TEST_ASSERT_EQUAL_FLOAT(4.0, quantize(4.2, 0.5));
    TEST_ASSERT_EQUAL_FLOAT(4.25, quantize(4.2, 0.25));
}

void test_strip_filename(void) {
    char string[] = "somefolder/file";
    strip_filename(string, ARRAY_LENGTH(string));
    TEST_ASSERT_FALSE(strcmp("somefolder/", string));
}

void test_has_suffix(void) {
    TEST_ASSERT_TRUE(has_suffix("astringsuffix", "suffix"));
    TEST_ASSERT_FALSE(has_suffix("astringsuffi", "suffix"));
    TEST_ASSERT_FALSE(has_suffix("astringsuffix", "gsuffi"));
}

void test_has_suffix_true_on_equal_strings(void) {
    TEST_ASSERT_TRUE(has_suffix("somestring", "somestring"));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_max_min);
    RUN_TEST(test_quantize);
    RUN_TEST(test_strip_filename);
    RUN_TEST(test_has_suffix);
    RUN_TEST(test_has_suffix_true_on_equal_strings);

    return UNITY_END();
}
