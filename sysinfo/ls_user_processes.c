/**
 * List all processes being run by user.
 * usage:
 * $ ls_user_processes 
 *
 * arg 1 - user name or defaults to `whoami`
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
#include "ugid_functions.h"

static Boolean
strStartsWith(const char *restrict str, const char *restrict prefix)
{
    while (*prefix) {
        if (*prefix++ != *str++)
            return 0;
    }
    return 1;
}

static void
listFilesByUserId(const char *dirpath, const uid_t userId)
{
    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(dirpath);
    char *status = "status";

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
            /* printf("Dir: %s \t #: %d\n", dp->d_name, pid); */
            FILE *stream;

            /* allocate absolute pathname buffer, add 1 for '/' */
            char *fullpath = malloc(strlen(dp->d_name) + strlen(dirpath) + 
                    strlen(status) + 2);
            if (fullpath == NULL) {
                errMsg("coult not get full path");
                continue;
            }

            /* create absolute file path */
            sprintf(fullpath, "%s/%s/%s", dirpath, dp->d_name, status);

            /* make sure dir still exists */
            if (!readdir(dirp)) {
                errMsg("%s has been closed", fullpath);
                continue;
            }

            stream = fopen(fullpath, "r");
            if (stream == NULL) {
                errMsg("could not open %s", fullpath);
                continue;
            }

            /* read into buffer */
            char buf[LINE_MAX];
            char nameBuf[LINE_MAX];
            char userIdStr[LOGIN_NAME_MAX + 2];

            while (fgets(buf, LINE_MAX, stream))
            {
                if (strStartsWith(buf, "Name"))
                    sprintf(nameBuf, "%s", buf);

                if (strStartsWith(buf, "Uid")) {
                    sprintf(userIdStr, "%d", userId);
                    /* @TODO regex for uid */
                    if (strstr(buf, userIdStr)) {
                        /* @TODO readdir again to verify process open */
                        printf("pid: %d\n", pid);
                        printf("%s", nameBuf);
                        printf("%s\n", buf);
                    }
                }

            }

            if (fclose(stream) != 0) {
                errMsg("could not close %s", fullpath);

                /* @TODO cleanup fullpath before loop? */
                continue;
            }
            free(fullpath);
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
    uid_t userId;
    char *userName;
    const char *procDir = "/proc";

    /* @TODO use `whoami` if no user provided */
    if (argc < 2 || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr("%s username", argv[0]);

    userName = argv[1];
    userId = userIdFromName(userName);

    listFilesByUserId(procDir, userId);
  
    exit(EXIT_SUCCESS);
}
