#include <unistd.h>
#include <fcntl.h>
#include <limits.h> /* PIPE_BUF */
#include "tlpi_hdr.h"


int main(void)
{
	int n, pipefd[2];
	char line[LINE_MAX];

	if (pipe(pipefd) == -1)
		errExit("pipe error");

	switch (fork()) {
		case -1:
			errExit("fork");

		case 0:     /* child */
					if (close(pipefd[1]) == -1)
						errExit("close");
					if ((n = read(pipefd[0], line, LINE_MAX)) == -1)
						errExit("read");
					if (write(STDOUT_FILENO, line, n) == -1)
						errExit("write");
				break;

		default:     /* parent */
					 if (close(pipefd[0]) == -1)
						 errExit("close");
					 if (write(pipefd[1], "Howdy World\n", 12) == -1)
						 errExit("write");
				 break;

	}

	exit(EXIT_SUCCESS);
}
