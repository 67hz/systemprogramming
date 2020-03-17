/**
 * open_stdin.c
 * use of open() with STD_IN only.
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"

#ifndef BUF_SIZE        /* allow "cc -D" to override defn */
#define BUF_SIZE 1024
#endif

int
main (int argc, char *argv[])
{
    int inputFd, outputFd;
    ssize_t numRead;
    char buf[BUF_SIZE];
    char *pathname = argv[1];

    /* Close file descriptor 0, guarantees file opens with 0 descriptor */
    if (close(STDIN_FILENO) == -1)      
        errExit("close");

    inputFd = open(pathname, O_RDONLY);
    if (inputFd == -1)
        errExit("opening file %s", pathname);

    /* outputFd = open(argv[2], O_RDWR, S_IRUSR | S_IWUSR); */
    /* if (outputFd == -1) */
    /*     errExit("openingFile %s", argv[2]); */
    /*  */
    /* while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) */
    /*     if (write(outputFd, buf, numRead) != numRead) */
    /*         fatal("could not write whole buffer"); */
    /*  */
    /* if (numRead == -1) */
    /*     errExit("read"); */
    /*  */
    /* if (close(inputFd) == -1) */
    /*     errExit("close input"); */
    /*  */
    /* if (close(outputFd) == -1) */
    /*     errExit("close output"); */
}
