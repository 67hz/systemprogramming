#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int
main(void)
{
	if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
		printf("cannot seek\n");
	else
		printf("seek OK\n");

	exit(EXIT_SUCCESS);
}
