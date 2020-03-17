/**
 * bad_exclusive.c
 *
 * incorrectly open a file leading to race conditions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"


int
main(int argc, char *argv[])
{

    int fd, opt;
    char *path;
    int withDelay = 0;

    while ((opt = getopt(argc, argv, "s")) != -1) {
        switch (opt) {
            case 's':
                withDelay = 1;
                break;
            default:
                usageErr("bad args: %s", argv[0]);
        }
    }

    path = argv[optind];

    fd = open(path, O_WRONLY);  /* Open 1: check if file exists */ 
    if (fd != -1)
    {
        printf("[PID %ld] File \"%s\" already exists\n",
                (long) getpid(), path);
        close(fd);
    } else
    {
        if (errno != ENOENT)        /* Failed for unexpected reason */
        {
            errExit("open");
        }
        else
        {
            /* WINDOW FOR FAILURE */
            printf("[PID %ld] File \"%s\' doesn't exist yet\n",
                    (long) getpid(), path);

            if (argc > 2)
            {
                sleep(5);
                printf("[PID %ld] Done sleeping\n", (long) getpid());
            }

            /* between open calls another process can open/create */
            fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            if (fd == -1)
                errExit("open");

            printf("[PID %ld] Created file \"%s\" exclusively\n",
                    (long) getpid(), path);      /* May not be true */

            if (close(fd) == -1)
                errExit("close file");


        }

        /* errExit(EXIT_SUCCESS); */
    }


}
