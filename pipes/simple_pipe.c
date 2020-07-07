#include <sys/wait.h>
#include <unistd.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 10


int
main (int argc, char *argv[])
{
    int pfd[2];
    char buf[BUF_SIZE];
    ssize_t num_read;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s string\n", argv[0]);

    if (pipe(pfd) == -1)
        errExit("pipe");

    switch (fork()) {
    case -1:
        errExit("fork");

    case 0:         /* child reads from pipe */
        if (close(pfd[1]) == -1)        /* write is unused */
            errExit("close - child");

        for (;;) {      /* read data from pipe, echo on STDOUT */
            num_read = read(pfd[0], buf, BUF_SIZE);
            if (num_read == -1)
                errExit("read");
            if (num_read == 0)
                break;      /* EOF */
            if (write(STDOUT_FILENO, buf, num_read) != num_read)
                fatal("child - partial/failed write");
        }

        write (STDOUT_FILENO, "\n", 1);
        if (close(pfd[0]) == -1)
            errExit("close");
        _exit(EXIT_SUCCESS);

    default:        /* parent - writes to pipe */
        if (close(pfd[0]) == -1)
            errExit("close - parent");

        if (write(pfd[1], argv[1], strlen(argv[1])) != strlen(argv[1]))
            fatal("parent - partial/failed write");

        if (close(pfd[1]) == -1)    /* child will see EOF */
            errExit("close");
        wait(NULL);     /* wait for child to finish */
        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
