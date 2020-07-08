#include <time.h>
#include "get_num.h"
#include "tlpi_hdr.h"

char *
getCurrentTime()
{
    time_t t;
    size_t s;
    struct tm *tm;
    static char buf[BUFSIZ];


    t = time(NULL); /* time since epoch in s */
    tm = localtime(&t);
    if (tm == NULL)
        perror("localtime");

    // %r = am.pm notation
    strftime(buf, BUFSIZ, "%b", tm);
    return buf;
}


int
main(int argc, char *argv[])
{
    int pfd[2];
    int j, dummy;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sleep-time...\n", argv[0]);

    
    setbuf(stdout, NULL);   /* make stdout unbuffered, since
                                terminate child with _exit() 
                                which does not flush open streams */

    printf("%s Parent started\n", getCurrentTime());

    if (pipe(pfd) == -1)
        errExit("pipe");

    for (j = 1; j < argc; j++) {

        switch (fork()) {
        case -1:
            errExit("fork %d", j);

        case 0:     /* child */
            if (close(pfd[0]) == -1)        /* read end is unused */
                errExit("close");

            /* child works, and lets parent know it is done */

            sleep(getInt(argv[j], GN_NONNEG, "sleep time"));
                                            /* simulate processing */
            printf("%s Child %d (PID=%ld) closing pipe\n",
                    getCurrentTime(), j, (long) getpid());
            if (close(pfd[1]) == -1)    /* close write end */
                errExit("close");

            /* child now carries on to do other things */

            _exit(EXIT_SUCCESS);

        default:    /* parent loops to create next child */
            break;
        }
    }

    /* Parent comes here; close write end of pipe so we can see EOF */

    if (close(pfd[1]) == -1)    /* write end unused by parent */
        errExit("close");

    /* Parent may do other work, then sync with children */

    /* blocks until all children have closed their FDs for write end of pipe  */
    if (read(pfd[0], &dummy, 1) != 0)
        fatal("parent did not get EOF");
    printf("%s Parent ready to go: \n", getCurrentTime());

    /* Parent can now carry on to other tasks */

    exit(EXIT_SUCCESS);
}
