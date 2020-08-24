#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#define MAX_ERROR_LEN   256      /* max length of str returned by strerror() */

static __thread char buf[MAX_ERROR_LEN];

char *
strerror(int err)
{
    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unkwon error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';
    }
    return buf;
}
