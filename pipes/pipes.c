#include <unistd.h>
#include <fcntl.h>
#include <limits.h> /* PIPE_BUF */
#include "tlpi_hdr.h"


int main(void)
{
    int n, pipefd[2];
    pid_t cpid;
    char line[LINE_MAX];

    if (pipe(pipefd) < 0)
        errExit("pipe error");

    if ( (cpid = fork()) < 0)
        errExit("fork");

    else if (cpid > 0) {     /* parent */
        close(pipefd[0]);
        write(pipefd[1], "Howdy World\n", 12);

    } else {
        close(pipefd[1]);
        n = read(pipefd[0], line, LINE_MAX);
        write(STDOUT_FILENO, line, n);
    }


    exit(EXIT_SUCCESS);
}
