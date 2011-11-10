
#include "test.h"


void test_mockart_just_init_finish(void);
void test_mockart_expect_entrance(void);

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
}
