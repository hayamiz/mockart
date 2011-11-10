
#include "mocking-target.h"

char *
call_strdup_inside(const char *str)
{
    return strdup(str);
}
