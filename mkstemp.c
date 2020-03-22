/**
 * mkstemp.c
 * example of the uses of mkstemp()
 * creates a temp file. 
 *
 * See also tmpfile()
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
    int openFlags = O_RDWR | O_CREAT | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR;

    /* last characters must be 'XXXXXX' */
    char template[] = "/tmp/somestringXXXXXX";
    fd = mkstemp(template);
    if (fd == -1)
      errExit("mkstemp");

    printf("Generated filename: %s\n", template);

    /* name removed immediately, file is removed after close() */
    unlink(template);

    if (close(fd) == -1)
      errExit("closing %s", template);

    exit(EXIT_SUCCESS);



}
