
#include "test.h"


void test_mockart_just_init_finish(void);
void test_mockart_mock_with_str_arg(void);
void test_mockart_mock_with_str_arg_fail(void);


void
test_mockart_just_init_finish(void)
{
    mockart_init();
    cut_assert_equal_int(0, mockart_finish());
}


/* dummy mocked strdup(3) */
char *
dummy_strdup(const char *str)
{
    puts("ok");
    mockart_do_entrance(__func__, str, NULL);

    return NULL;
}

void
test_mockart_mock_with_str_arg(void)
{
    mockart_init();

    mockart_expect_entrance("dummy_strdup",
                            MOCK_ARG_STR, "hello world",
                            NULL);
    cut_assert_null(dummy_strdup("hello world")); // dummy mocked strdup

    cut_assert_equal_int(0, mockart_finish(),
                         cut_message("%s", mockart_failure_message()));
}

void
test_mockart_mock_with_str_arg_fail(void)
{
    mockart_init();

    mockart_expect_entrance("dummy_strdup",
                            MOCK_ARG_STR, "hello world",
                            NULL);

    cut_assert_equal_int(-1, mockart_finish());
}
