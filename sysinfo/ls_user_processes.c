/**
 * List all processes being run by user.
 *
 * readdir or nftw for traversal??? - readdir - only 1 level deep in /proc/pid
 * use stat to check for user id of owner???
 * read contents of /proc/pid/status to get name? not available in stat
 * get line starting with Uid:
 * if matches current process Uid, (save)/display result
 *
 * see strstr for searching file if not using stat
 *
 */


#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "tlpi_hdr.h"


static void
listFiles(const char *dirpath)
{
    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(dirpath);

    if (dirp == NULL) {
        errMsg("opendir failed: %s", dirpath);
        return;
    }


    /* loop through entries in dir stream */

    for (;;) {
        /* set errno to 0 before calling readdir to distinguish
         * EOS from error
         * If EODirStream reached, NULL returned and errno not changed
         * On error, NULL returned and errno set.
         * see man readdir for more info
         */
        errno = 0;
        int pid;

        dp = readdir(dirp);

        if (dp == NULL) /* EODS */
            break;

        /* only want dirs that are numbered && > 0 */
        pid = strtol(dp->d_name, NULL, 10); /* returns 0 if no digits */
        if (pid) {
            printf("Dir: %s \t #: %d\n", dp->d_name, pid);

            /* read contents for matching uid */
            FILE *stream;
            if (!fopen(dp->d_name, "r")) {
                errMsg("could not open %s", dp->d_name);
                continue;
            }



        }
    }

    if (errno != 0)
        errExit("readdir");

    if (closedir(dirp) == -1)
        errMsg("closeDir");
}



int main
(int argc, char *argv[])
{
    const char *procDir = "/proc";

    listFiles(procDir);


  
    exit(EXIT_SUCCESS);
}
