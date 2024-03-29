#include "mockart.h"
#include <glib.h>

typedef struct {
    // file and lineno at where mockart_expect_entrance was called
    const char *file;
    int lineno;

    GList *mock_args;
} sched_entry_t;

static char *mockart_sprintf(const char *fmt, ...){
    int n, size = 100;
    char *p;
    va_list args;

    if ((p = malloc(size)) == NULL) {
        return NULL;
    }

    for(;;){
        va_start(args, fmt);
        n = vsnprintf(p, size, fmt, args);
        va_end(args);

        if (n >= size) {
            size *= 2;
            p = realloc(p, size);
        } else {
            return p;
        }
    }

    return NULL;
}


/* This variable holds infomation about expected function calls and
 * their arguments.
 * key: function name (string)
 * value: GList of GList of mockart_arg_t
 */
static GHashTable *entrance_schedule_table = NULL;

#define NR_BACKTRACE 128
static char *fail_msg = NULL;
static void *fail_bt_buffer[NR_BACKTRACE];
static char **fail_bt_symbols;
static int fail_bt_nr = 0;
static GList *expect_free_list = NULL;
static GList *freed_list = NULL;

static void
__mockart_free_hook_func(void *ptr, const void *caller)
{
    freed_list = g_list_append(freed_list, ptr);
}

#define __collect_backtrace()                                           \
    {                                                                   \
        fail_bt_nr = backtrace(fail_bt_buffer, NR_BACKTRACE);           \
        fail_bt_symbols = backtrace_symbols(fail_bt_buffer, fail_bt_nr); \
    }

static char *
mockart_bt_to_str(void)
{
    GString *str;
    char *ret;
    int i;

    str = g_string_new("Backtrace:\n");
    if (fail_bt_nr == 0) {
        g_string_append(str, "  No backtrace available.");
    } else {
        for(i = 0; i < fail_bt_nr; i++) {
            g_string_append_printf(str, "  %s\n", fail_bt_symbols[i]);
        }
    }

    ret = str->str;
    g_string_free(str, FALSE);

    return ret;
}

void mockart_init(void)
{
    if (entrance_schedule_table != NULL) {
        g_hash_table_destroy(entrance_schedule_table);
    }

    if (fail_msg != NULL) {
        free(fail_msg);
    }

    if (expect_free_list != NULL) {
        g_list_free(expect_free_list);
        expect_free_list = NULL;
    }

    if (freed_list != NULL) {
        g_list_free(freed_list);
        freed_list = NULL;
    }

    fail_msg = NULL;
    entrance_schedule_table = g_hash_table_new(g_str_hash, g_str_equal);
    __free_hook = __mockart_free_hook_func;
}

static void
mockart_finish_iterator(void *_key, void *_value, void *_user_data)
{
    const char *fname = (const char *) _key;
    GList *sched_list = (GList *) _value;
    sched_entry_t *sched_entry;
    GList *mock_args;
    mockart_arg_t *mock_arg;
    GString *msg;

    if (sched_list == NULL) {
        return;
    }

    if (fail_msg != NULL) {
        return;
    }

    msg = g_string_new("(");
    sched_entry = (sched_entry_t *)sched_list->data;
    mock_args = sched_entry->mock_args;

    for(; mock_args != NULL; mock_args = mock_args->next) {
        mock_arg = (mockart_arg_t *) mock_args->data;
        switch(mock_arg->type){
        case MOCK_ARG_SKIP:
            g_string_append_printf(msg, "(skipped), ");
            break;
        case MOCK_ARG_INT:
            g_string_append_printf(msg, "%d, ", mock_arg->u._int);
            break;
        case MOCK_ARG_LONG:
            g_string_append_printf(msg, "%ld, ", mock_arg->u._long);
            break;
        case MOCK_ARG_STR:
            g_string_append_printf(msg, "\"%s\", ", mock_arg->u._str);
            break;
        case MOCK_ARG_PTR:
            g_string_append_printf(msg, "%p, ", mock_arg->u._ptr);
            break;
        default:
            fprintf(stderr,
                    "mockart_finish_iterator: unimplemented type %d\n",
                    mock_arg->type);
            exit(EXIT_FAILURE);
        }
    }
    if (msg->len == 1){
        g_string_append_printf(msg, ")");
    } else {
        msg = g_string_truncate(msg, msg->len - 2);
        g_string_append_printf(msg, ")");
    }

    fail_msg = mockart_sprintf("Expected function call: %s %s",
                               fname, msg->str);
    g_string_free(msg, TRUE);
}

int
mockart_finish(void)
{
    if (entrance_schedule_table == NULL) {
        return 0;
    }

    g_hash_table_foreach(entrance_schedule_table,
                         mockart_finish_iterator,
                         NULL);

    g_hash_table_destroy(entrance_schedule_table);
    entrance_schedule_table = NULL;

    if (fail_msg == NULL) {
        GList *expect_list;
        for(expect_list = expect_free_list;
            expect_list != NULL;
            expect_list = expect_list->next){
            if (g_list_find(freed_list, expect_list->data) == NULL) {
                fail_msg = mockart_sprintf("<%p> is expected to be free(3)ed, but actually not.",
                                           expect_list->data);
                break;
            }
        }
    }

    if (fail_msg != NULL) {
        return -1;
    }

    return 0;
}

bool
mockart_enabled(void)
{
    return entrance_schedule_table != NULL;
}

const char *
mockart_failure_message(void)
{
    return fail_msg;
}

void
__mockart_expect_entrance(const char *file, int lineno, const char *fname, ...)
{
    va_list args;
    GList *sched_list;
    sched_entry_t *sched_entry;
    GList *mock_args;

    mockart_arg_t *mock_arg;
    mockart_arg_type_t mock_arg_type;

    sched_list = g_hash_table_lookup(entrance_schedule_table, fname);

    sched_entry = malloc(sizeof(sched_entry_t));
    sched_entry->file = file;
    sched_entry->lineno = lineno;
    mock_args = NULL;

    va_start(args, fname);

    for(;;) {
        mock_arg_type = va_arg(args, mockart_arg_type_t);
        if (mock_arg_type == 0) {
            break;
        }

        mock_arg = malloc(sizeof(mockart_arg_t));
        mock_arg->type = mock_arg_type;
        switch(mock_arg_type) {
        case MOCK_ARG_SKIP:
            va_arg(args, void *);
            break;
        case MOCK_ARG_INT:
            mock_arg->u._int = va_arg(args, int);
            break;
        case MOCK_ARG_LONG:
            mock_arg->u._long = va_arg(args, long);
            break;
        case MOCK_ARG_STR:
            mock_arg->u._str = va_arg(args, const char *);
            break;
        case MOCK_ARG_PTR:
            mock_arg->u._ptr = va_arg(args, void *);
            break;
        default:
            fprintf(stderr,
                    "mockart_assert_will_call: unimplemented type %d\n",
                    mock_arg_type);
            exit(EXIT_FAILURE);
        }

        mock_args = g_list_append(mock_args, mock_arg);
    }

    sched_entry->mock_args = mock_args;
    sched_list = g_list_append(sched_list, sched_entry);

    g_hash_table_insert(entrance_schedule_table, (char *) fname, sched_list);

    va_end(args);
}

void
mockart_do_entrance(const char *fname, ...)
{
    GList *sched_list;
    sched_entry_t *sched_entry;
    GList *mock_args;

    mockart_arg_t *mock_arg;
    va_list args;
    int i;
    char *bt_str;

    int int_arg;
    long long_arg;
    const char *str_arg;
    void *ptr_arg;

    GString *_fail_msg;

    if (entrance_schedule_table == NULL)
        return;

    /*
     * fail_msg != NULL means some entrance failures have already
     * occured.  In this case, do not overwrite fail_msg because I
     * want to focus on the first failure.
     */
    if (fail_msg != NULL) {
        return;
    }

    if (NULL == (sched_list = g_hash_table_lookup(entrance_schedule_table, fname))) {
        return;
    }

    __collect_backtrace();
    bt_str = mockart_bt_to_str();

    _fail_msg = g_string_new("");

    sched_entry = (sched_entry_t *) sched_list->data;
    sched_list = g_list_remove(sched_list, sched_entry);
    g_hash_table_insert(entrance_schedule_table, (char *) fname, sched_list);
    mock_args = sched_entry->mock_args;

    va_start(args, fname);

    for(i = 1;
        mock_args != NULL && _fail_msg->len == 0;
        mock_args = mock_args->next, i++)
    {
        mock_arg = (mockart_arg_t *) mock_args->data;
        switch(mock_arg->type) {
        case MOCK_ARG_SKIP: // no check
            va_arg(args, void *);
            break;
        case MOCK_ARG_INT:
            int_arg = va_arg(args, int);
            if (int_arg != mock_arg->u._int) {
                g_string_printf(_fail_msg, "Failure on %d-th argument of '%s': "
                                "expected <%d> but actually <%d>\n",
                                i, fname,
                                mock_arg->u._int,
                                int_arg);
            }
            break;
        case MOCK_ARG_LONG:
            long_arg = va_arg(args, long);
            if (mock_arg->u._long != long_arg) {
                g_string_printf(_fail_msg, "Failure on %d-th argument of '%s': "
                                "expected <%ld> but actually <%ld>\n",
                                i, fname,
                                mock_arg->u._long,
                                long_arg);
            }
            break;
        case MOCK_ARG_STR:
            str_arg = va_arg(args, const char *);
            if (strcmp(mock_arg->u._str, str_arg) != 0) {
                g_string_printf(_fail_msg, "Failure on %d-th argument of '%s': "
                                "expected <\"%s\"> but actually <\"%s\">\n",
                                i, fname,
                                mock_arg->u._str,
                                str_arg);
            }
            break;
        case MOCK_ARG_PTR:
            ptr_arg = va_arg(args, void *);
            if (mock_arg->u._ptr != ptr_arg) {
                g_string_printf(_fail_msg, "Failure on %d-th argument of '%s': "
                                "expected <%p> but actually <%p>\n",
                                i, fname,
                                mock_arg->u._ptr,
                                ptr_arg);
            }
            break;
        default:
            fprintf(stderr,
                    "%s: unimplemented type %d\n",
                    __func__,
                    mock_arg->type);
            exit(EXIT_FAILURE);
        }
    }

    va_end(args);

    if (_fail_msg->len > 0) {
        g_string_append_printf(_fail_msg,
                               "  Expected at %s:%d\n",
                               sched_entry->file, sched_entry->lineno);
        g_string_append(_fail_msg, bt_str);
        fail_msg = strdup(_fail_msg->str);
    }

    g_string_free(_fail_msg, TRUE);
    g_list_free(sched_entry->mock_args);
    free(sched_entry);
    free(bt_str);
}

void
mockart_expect_free(void *ptr)
{
    expect_free_list = g_list_append(expect_free_list, ptr);
}
