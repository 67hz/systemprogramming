/**
 * open.c
 * example of the uses of open()
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"


int
main(int argc, char *argv[])
{
    int fd;


    /* fd = open("startup", O_RDONLY); */
    /* if (fd == -1) */
    /*     errExit("open 'startup'"); */

    /* Open new or existing file for reading and writing, truncating to zero
     * bytes; file perms read+write for owner, nothing for others.
     */

    int openFlags = O_RDWR | O_CREAT | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR;

    fd = open("myfile", openFlags, filePerms);
    if (fd == -1)
        errExit("open 'myfile'");


    /* Open new or existing file for writing; writes should always append
     * to end of file.
     */

    openFlags = O_WRONLY | O_CREAT | O_TRUNC | O_APPEND;
    filePerms = S_IRUSR | S_IWUSR;
    fd = open("w.log", openFlags, filePerms);
    if (fd == -1)
        errExit("open 'w.log'");



}
