/**
 * readv.c
 * demo of vectorized reads
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "lib/tlpi_hdr.h"


int
main(int argc, char *argv[])
{
    int fd;
    struct iovec iov[3];
    struct stat myStruct;       /* first buffer */
    int x;                      /* second buffer */
#define STR_SIZE 100
    char str[STR_SIZE];         /* third buffer */
    ssize_t numRead, totRequired;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file \n", argv[0]);

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        errExit("open %s", argv[1]);

    totRequired = 0;

    iov[0].iov_base = &myStruct;            /* base is start address */
    iov[0].iov_len = sizeof(struct stat);   /* # bytes to read */
    totRequired += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    totRequired += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    totRequired += iov[2].iov_len;


    /* see man preadv for multithreaded version */
    numRead = readv(fd, iov, 3);
    if (numRead == -1)
        errExit("readv");

    printf("numRead %ld\n", (long)numRead);
    printf("totRequired %ld\n", (long)totRequired);
    if (numRead < totRequired)
        errExit("Read fewer bytes than requested");

    printf("total bytes requested: %ld;\tbytes_read: %ld\n",
            (long) totRequired, (long) numRead);

    exit(EXIT_SUCCESS);
}
