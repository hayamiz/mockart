
#include "test.h"


void test_mockart_just_init_finish(void);
void test_mockart_mock_with_str_arg(void);
void test_mockart_mock_with_str_arg_fail(void);
void test_mockart_override_memcpy(void);

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

/* mocked memcpy */
void *
memcpy(void *dest, const void *src, size_t sz)
{
    void *(*real_memcpy)(void *, const void *, size_t) =
        (void *(*)(void *, const void *, size_t))dlsym(RTLD_NEXT, "memcpy");

    mockart_do_entrance(__func__, dest, src, sz, NULL);

    puts("memcpy mocked.");

    return real_memcpy(dest, src, sz);
}

void
test_mockart_override_memcpy(void)
{
    mockart_init();

    char buf1[64], buf2[64];
    mockart_expect_entrance("memcpy",
                            MOCK_ARG_PTR, buf1,
                            MOCK_ARG_PTR, buf2,
                            MOCK_ARG_INT, 64,
                            NULL);
    memcpy(buf1, buf2, 64);

    cut_assert_equal_int(0, mockart_finish());
}
