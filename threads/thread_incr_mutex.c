#include <pthread.h>
#include "tlpi_hdr.h"

static volatile int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

struct threadData {
    char name[16];
    int num_loops;
};

static void *
threadFunc(void *arg)
{
    struct threadData data = *(struct threadData *)arg;

    int loc, j, s;

    for (j = 0; j < data.num_loops; j++) {
#ifndef NOLOCK
        s = pthread_mutex_lock(&mtx);
        if (s != 0)
            errExitEN(s, "pthread_mutex_lock");
#endif
        loc = glob;
        loc++;
        glob = loc;
        printf("thread: %s  glob: %d\n", data.name, glob);

#ifndef NOLOCK
        s = pthread_mutex_unlock(&mtx);
        if (s != 0)
            errExitEN(s, "pthread_mutex_unlock");
#endif
    }

    return NULL;
}

int
main(int argc, char **argv)
{
    pthread_t t1, t2;
    int loops, s;

    loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 1000000;

    struct threadData t1Data = (struct threadData){.name="worker-bob", .num_loops=loops};
    struct threadData t2Data = (struct threadData){.name="worker-hill", .num_loops=loops};

    s = pthread_create(&t1, NULL, threadFunc, &t1Data);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_create(&t2, NULL, threadFunc, &t2Data);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
