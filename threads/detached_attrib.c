#include <pthread.h>
#include "tlpi_hdr.h"

static void *
threadFunc(void *arg)
{
    char *s = (char *) arg;
    printf("%s\n", s);
    return (void *) strlen(s);
}

int
main(int argc, char **argv)
{
    pthread_t thr;
    pthread_attr_t attr;
    void *res;
    int s;

    s = pthread_attr_init(&attr);
    if (s != 0)
        errExitEN(s, "pthread_attr_init");

    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (s != 0)
        errExitEN(s, "pthread_attr_setdetachstate");

    s = pthread_create(&thr, &attr, threadFunc, "testing");
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_attr_destroy(&attr);
    if (s != 0)
        errExitEN(s, "pthread_attr_destroy");

    exit(EXIT_SUCCESS);
}
