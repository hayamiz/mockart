#ifndef CUTTER_MOCK_H
#define CUTTER_MOCK_H

#define _GNU_SOURCE

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <execinfo.h>

#include "micbench-test-utils.h"

typedef enum {
    MOCK_ARG_SKIP = 1,
    MOCK_ARG_INT,
    MOCK_ARG_LONG,
    MOCK_ARG_PTR,
} mb_mock_arg_type_t;

typedef struct {
    mb_mock_arg_type_t type;
    union {
        int _int; // MOCK_ARG_INT
        long _long; // MOCK_ARG_LONG
        void *_ptr; // MOCK_ARG_PTR
    } u;
} mb_mock_arg_t;

/* ---- utility functions ---- */

void mockart_init(void);
void mockart_finish(void);

/* mb_assert_will_call(const char *fname,
 *                     mb_mock_arg_type_t arg_type1, arg1,
 *                     ..., NULL)
 */
void mockart_assert_will_call(const char *fname, ...);

void mockart_check(const char *fname, ...);


#endif
