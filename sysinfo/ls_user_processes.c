/**
 * List all processes being run by user.
 * usage:
 * $ ls_user_processes 
 *
 * arg 1 - user name or defaults to getlogin()
 *
 * readdir or nftw for traversal??? - readdir - only 1 level deep in /proc/pid
 * use stat to check for user id of owner???
 * read contents of /proc/pid/status to get name/uid? not available in stat
 *
 */
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>   /* for stat macros */
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
                errMsg("%d has been closed", pid);
                continue;
            }

            stream = fopen(fullpath, "r");
            if (stream == NULL) {
                errMsg("could not open %s", fullpath);
                continue;
            }

            /* read into buffer */
            char buf[LINE_MAX];
            struct stat statBuf;

            if (stat(fullpath, &statBuf) < 0) {
                errMsg("stat %s", fullpath);
                continue;
            }

            /* verify process matches CLI-arg provided user by userId
             * and is a regular file */
            if (statBuf.st_uid != userId || !S_ISREG(statBuf.st_mode))
                continue;

            while (fgets(buf, LINE_MAX, stream))
            {

                if (strStartsWith(buf, "Name")) {
                    buf[strlen(buf) - 1] = '\0'; /* replace fgets \n */                 
                    printf("%s", buf);
                    printf("\tpid: %d\n", pid);
                    break;
                }
            }

            if (fclose(stream) != 0) {
                errMsg("could not close %s", fullpath);
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

    if (argc > 2 || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr("%s username\n", argv[0]);

    if (argc < 2)
        userName = getlogin();
    else 
        userName = argv[1];

    if ((userId = userIdFromName(userName)) == -1)
        usageErr("%s is not a valid user\n", userName);

    listFilesByUserId(procDir, userId);
  
    exit(EXIT_SUCCESS);
}
