#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include "tlpi_hdr.h"


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif /* ifndef BUF_SIZE */

int main(int argc, char **argv)
{
    int inputFd, outputFd, cloneFd;
    ssize_t numRead;
    char buf[BUF_SIZE];
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                        S_IROTH | S_IWOTH;

    /* Open file and pipe the output to stdin and second file */

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("open %s", argv[1]);
    
    outputFd = open(argv[2], O_RDWR | O_CREAT, filePerms);
    if (outputFd == -1)
        errExit("open %s", argv[2]);

    /* duplicate target file "outputFd" with stderr */
    /* eq of piping file to stderr */
    cloneFd = dup2(STDERR_FILENO, outputFd);
    if (cloneFd == -1)
        errExit("dup2");

    if (close(STDERR_FILENO) == -1)
        errExit("close %s", argv[2]);

    /* writing to output file but now that is piped to STDERR */
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
        if (write(outputFd, buf, numRead) != numRead)
            fatal("could not write whole buffer");

    if (numRead == -1)
        errExit("read");

    if (close(outputFd) == -1)
        errExit("close output");

    exit(EXIT_SUCCESS);




}
