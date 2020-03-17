/**
 * copy.c
 * copy src file into dest
 *
 * CLI options:
 * -s copy sparse file with holes
 *
 * Example:
 * copy srcfile destfile -s
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"

#ifndef BUF_SIZE		/* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[])
{
	int inputFd, outputFd, openFlags, opt;
	mode_t filePerms;
	ssize_t numRead;
	char buf[BUF_SIZE];
    char *srcPath, *destPath;
    int isSparseCopy = 0;

	if (argc > 4 || strcmp(argv[1], "--help") == 0)
		usageErr("%s old-file new-file\n", argv[0]);

    while ((opt = getopt(argc, argv, "s")) != -1) {
        switch (opt) {
            case 's':
                fprintf(stdout, "%s passed sparse flag\n", optarg);
                isSparseCopy = 1;
                break;
            default:
                usageErr("%s old-file new-file\n", argv[0]);
        }
    }

    srcPath = argv[optind];
    destPath = argv[++optind];

	/* Open input and output files */

	inputFd = open(srcPath, O_RDONLY);
	if (inputFd == -1)
		errExit("opening file %s", srcPath);

	openFlags = O_CREAT | O_WRONLY | O_TRUNC;
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWOTH;	/* rw-rw-rw- */
	outputFd = open(destPath, openFlags, filePerms);
	if (outputFd == -1)
		errExit("opening file %s", destPath);

	/* Transfer data until we encounter end of input error */

    int i, numHoles;

	while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
        if (isSparseCopy) {
            for (i = 0; i < numRead; i++) {
                /* holes are populated with null char '\0' so skip if sparse */
                if (buf[i] == '\0')
                {
                    numHoles++;
                    continue;
                } else if (numHoles > 0)
                {
                    lseek(outputFd, numHoles, SEEK_CUR);
                    numHoles = 0;
                }
                if (write(outputFd, &buf[i], 1) != 1)
                    fatal("could not write byte to file in sparse mode");
            }
        } else {
            if (write(outputFd, buf, numRead) != numRead)
                fatal("could not write whole buffer");
        }

	if (numRead == -1)
		errExit("read");

	if (close(inputFd) == -1)
		errExit("close input");

	if (close(outputFd) == -1)
		errExit("close output");

	exit(EXIT_SUCCESS);
}
