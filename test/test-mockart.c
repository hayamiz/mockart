
#include "test.h"


void test_mockart_just_init_finish(void);
void test_mockart_expect_entrance(void);
void test_mockart_expect_entrance_fail(void);
void test_mockart_expect_entrance_order(void);
void test_mockart_expect_free(void);

#define MOCKART_FINISH() \
    cut_assert_equal_int(0, mockart_finish(), cut_message("%s", mockart_failure_message()));

void
test_mockart_just_init_finish(void)
{
    mockart_init();
    cut_assert_equal_int(0, mockart_finish());
}

void
test_mockart_expect_entrance(void)
{
    mockart_init();

    mockart_expect_entrance("test",
                            MOCK_ARG_INT, 1,
                            MOCK_ARG_STR, "foo",
                            MOCK_ARG_PTR, NULL,
                            MOCK_ARG_SKIP, NULL,
                            NULL);
    mockart_do_entrance("test", 1, "foo", NULL, 123, NULL);

    cut_assert_equal_int(0, mockart_finish());
    cut_assert_null(mockart_failure_message());
}


void
test_mockart_expect_entrance_fail(void)
{
    mockart_init();

    int expect_lineno;

    mockart_expect_entrance("test",
                            MOCK_ARG_INT, 1,
                            MOCK_ARG_STR, "foo",
                            MOCK_ARG_PTR, NULL,
                            MOCK_ARG_SKIP, NULL,
                            NULL); expect_lineno = __LINE__;
    mockart_do_entrance("test", 2, "foo", NULL, 123, NULL);
    cut_assert_not_equal_int(0, mockart_finish());
    cut_assert_match("^Failure on 1-th argument of 'test': expected <1> but actually <2>",
                     mockart_failure_message());
    cut_assert_match(cut_take_printf("^  Expected at test-mockart.c:%d",
                                     expect_lineno),
                     mockart_failure_message());
    cut_assert_match("Backtrace:\n", mockart_failure_message());
    cut_assert_match("\\btest_mockart_expect_entrance_fail\\b",
                     mockart_failure_message());

    mockart_init();
    mockart_expect_entrance("test",
                            MOCK_ARG_INT, 1,
                            MOCK_ARG_STR, "foo",
                            MOCK_ARG_PTR, NULL,
                            MOCK_ARG_SKIP, NULL,
                            NULL);
    mockart_do_entrance("test", 1, "bar", NULL, 123, NULL);
    cut_assert_not_equal_int(0, mockart_finish());
    cut_assert_match("^Failure on 2-th argument of 'test': expected <\"foo\"> but actually <\"bar\">",
                            mockart_failure_message());

    mockart_init();
    mockart_expect_entrance("test",
                            MOCK_ARG_INT, 1,
                            MOCK_ARG_STR, "foo",
                            MOCK_ARG_PTR, NULL,
                            MOCK_ARG_SKIP, NULL,
                            NULL);
    mockart_do_entrance("test", 1, "foo", (void *)0x64, 123, NULL);
    cut_assert_not_equal_int(0, mockart_finish());
    cut_assert_match("^Failure on 3-th argument of 'test': expected <\\(nil\\)> but actually <0x64>",
                            mockart_failure_message());

    mockart_init();
    mockart_expect_entrance("test",
                            MOCK_ARG_INT, 1,
                            MOCK_ARG_STR, "foo",
                            MOCK_ARG_PTR, NULL,
                            MOCK_ARG_SKIP, NULL,
                            NULL);
    mockart_do_entrance("test", 2, "bar", (void *)0x64, 123, NULL);
    cut_assert_not_equal_int(0, mockart_finish());
    cut_assert_match("^Failure on 1-th argument of 'test': expected <1> but actually <2>",
                            mockart_failure_message());
}

void
test_mockart_expect_entrance_order(void)
{
    mockart_init();

    mockart_expect_entrance("test",
                            MOCK_ARG_INT, 1,
                            NULL);
    mockart_expect_entrance("test",
                            MOCK_ARG_INT, 2,
                            NULL);

    mockart_do_entrance("test", 2, NULL);
    mockart_do_entrance("test", 1, NULL);

    cut_assert_not_equal_int(0, mockart_finish());
    cut_assert_match("^Failure on 1-th argument of 'test': expected <1> but actually <2>",
                            mockart_failure_message());
}

void
test_mockart_expect_free(void)
{
    void *ptr;

    // mockart_finish return -1 if ptr is not freed
    mockart_init();
    ptr = malloc(64);
    mockart_expect_free(ptr);
    cut_assert_not_equal_int(0, mockart_finish());

    free(ptr);

    // successfull case
    mockart_init();
    ptr = malloc(64);
    mockart_expect_free(ptr);
    free(ptr);
    MOCKART_FINISH();
}
