#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif




int
main(int argc, char **argv)
{
    int openFd, flags, accessMode;
    char *buf[BUFFER_SIZE];

    openFd = open(argv[1], O_CREAT | O_RDWR, S_IRUSR);
    if (openFd == -1)
        errExit("open %s", argv[1]);

    flags = fcntl(openFd, F_GETFL);
    if (flags == -1)
        errExit("fcntl");

    /* use mask to check flags with constant O_ACCMODE */
    accessMode = flags & O_ACCMODE;
    if (accessMode == O_WRONLY || accessMode == O_RDWR)
        printf("file is writeable\n");
    else if (accessMode == O_RDONLY)
        printf("file is read only\n");

    /* modify to append */
    flags |= O_APPEND;
    if (fcntl(openFd, F_SETFL, flags) == -1)
        printf("fcntl");


    if (close(openFd) == -1)
        errExit("close %s", argv[1]);

    exit(EXIT_SUCCESS);
}
