#ifndef CUTTER_MOCK_H
#define CUTTER_MOCK_H

#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <execinfo.h>

#include <glib.h>
#include <malloc.h>

typedef enum {
    MOCK_ARG_SKIP = 1,
    MOCK_ARG_INT,
    MOCK_ARG_LONG,
    MOCK_ARG_STR,
    MOCK_ARG_PTR,
} mockart_arg_type_t;

typedef struct {
    mockart_arg_type_t type;
    union {
        int _int; // MOCK_ARG_INT
        long _long; // MOCK_ARG_LONG
        const char *_str; // MOCK_ARG_STR
        void *_ptr; // MOCK_ARG_PTR
    } u;
} mockart_arg_t;

/* ---- utility functions ---- */

void mockart_init(void);

/* returned value: 0 if no failure, -1 if any failures */
int mockart_finish(void);

bool mockart_enabled(void);

const char *mockart_failure_message(void);

/* mb_assert_will_call(const char *fname,
 *                     mockart_arg_type_t arg_type1, arg1,
 *                     ..., NULL)
 */
void mockart_expect_entrance(const char *fname, ...);

void mockart_do_entrance(const char *fname, ...);

void mockart_expect_free(void *ptr);

#endif
