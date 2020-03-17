/**
 * tee.c
 *
 * Usage: tee file {-a}...
 *
 * This program reads stdin and writes a copy to stdout and to
 * the file named by its CLI arg. This should mimic system tee.
 *
 * CLI options:
 *  -a Append to file if it already exists
 *
 * Example:
 *  tee myfile.txt -a
 */
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"

#ifndef BUF_SIZE /* Allow "cc -D" to override defintion */
#define BUF_SIZE 1024
#endif

int
main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags, opt;
    openFlags = O_CREAT | O_RDWR | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                        S_IROTH | S_IWOTH;
    char *path;

    ssize_t numRead;
    char* buf[BUF_SIZE];

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s filetowrite\n", argv[0]);


    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
        case 'a':
            fprintf(stdout, "%d passed append flag\n", opt);
            openFlags = O_CREAT | O_RDWR | O_APPEND;
            break;
        default: /* '?' */
            usageErr("%s %s -a\n", argv[0], argv[1]);
        }
    }

    path = argv[optind];

    outputFd = open(path, openFlags, filePerms);
    if (outputFd == -1)
        errExit("open %s", path);

    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
    {
        if (write(outputFd, buf, numRead) != numRead)
            fatal("could not write whole buffer");
        else {
            if (write(STDIN_FILENO, buf, numRead) != numRead)
                fatal("could not write buffer to stdin");
        }
    }

    if (numRead == -1)
        errExit("read");

    if (close(outputFd) == -1)
        errExit("close output");
    
    exit(EXIT_SUCCESS);
}

