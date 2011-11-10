
#include "test.h"
#include "mocking-target.h"

/* mocked strdup(3) */

char *
strdup(const char *str){
    mockart_do_entrance(__func__, str, NULL);

    return NULL;
}

/* test case */

void
test_mocked_strdup(void)
{
    mockart_init();

    mockart_expect_entrance("strdup",
                            MOCK_ARG_STR, "hello world",
                            NULL);
    call_strdup_inside("hello world");

    cut_assert_equal_int(0, mockart_finish(),
                         cut_message("%s",
                                     mockart_failure_message()));
}
