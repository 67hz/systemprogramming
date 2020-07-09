#include <sys/wait.h>
#include <unistd.h>
#include "tlpi_hdr.h"


int main(int argc, char *argv[])
{
    int pfd[2];
    pid_t pid = getpid();
    pid_t cpid;

    if (pipe(pfd) == -1)
        errExit("pipe");

    /* first child: exec 'ls' to write to pipe */
    switch (cpid = fork()) {
    case -1:
        errExit("fork %ld", (long) cpid);

    case 0:     /* child */
        if (close(pfd[0]) == -1)        /* close read end */
            errExit("close write end: %ld", (long) cpid);

        /* bind STDOUT to write end of pipe */
        if (pfd[1] != STDOUT_FILENO)
            if (dup2(pfd[1], STDOUT_FILENO) == -1)
                errExit("dup2 %ld", (long) cpid);

        /* close extra write end */
        if (close(pfd[1]) == -1)
            errExit("close pfd[1] %ld", (long) cpid);

        execlp("ls", "ls", (char *) NULL);  /* writes to pipe */
        errExit("execlp ls %ld", (long) cpid);

    default:        /* parent fails through to create next child */
        break;
    }

    /* second child: exec 'wc' to read from pipe */
    switch (cpid = fork()) {
    case -1:
        errExit("fork %ld", (long) cpid);

    case 0:     /* child */
        if (close(pfd[1]) == -1)        /* close write end */
            errExit("close read end: %ld", (long) cpid);

        /* bind STDIN to read end of pipe */
        if (pfd[0] != STDIN_FILENO)
            if (dup2(pfd[0], STDIN_FILENO) == -1)
                errExit("dup2 %ld", (long) cpid);

        /* close read end */
        if (close(pfd[0]) == -1)
            errExit("close pfd[0]: %ld", (long) cpid);

        execlp("wc", "wc", "-l", (char *) NULL);  /* reads from stdin */
        errExit("execlp 'wc': %ld", (long) cpid);

    default:        /* parent fails through */
        break;
    }

    /* parent closes unused FDs for pipe and waits for children */
    if (close(pfd[0]) == -1)
        errExit("close prd[0] %d", (long) pid); 
    if (close(pfd[1]) == -1)
        errExit("close prd[1] %d", (long) pid); 

    if (wait(NULL) == -1)
        errExit("wait 1 %d", (long) pid);
    if (wait(NULL) == -1)
        errExit("wait 2 %d", (long) pid);


    exit(EXIT_SUCCESS);
}
