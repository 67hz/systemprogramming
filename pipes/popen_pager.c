#include "tlpi_hdr.h"
#include <sys/wait.h>

#define PAGER       "${PAGER:-more}"        /* env pager else use more */
#define MAXLINE 100

int main(int argc, char *argv[])
{
    char    line [MAXLINE];
    FILE    *fpin, *fpout;

    if (argc != 2)
        usageErr("./popen_pager <pathname>\n");

    if ( (fpin = fopen(argv[1], "r")) == NULL)
        errExit("Cannot open %s", argv[1]);

    if ( (fpout = popen (PAGER, "w")) == NULL)
        errExit ("popen");

    /* copy argv[1] to pager */
    while (fgets(line, MAXLINE, fpin) != NULL) /* get input file */
        if (fputs(line, fpout) == EOF)          /* and put to pager */
            errExit("fputs error to pipe");

    if (ferror(fpin))
        errExitEN(errno, "fgets error: %d\n");

    if (pclose(fpout) == -1)
        errExit("pclose");

    exit(EXIT_SUCCESS);
}

