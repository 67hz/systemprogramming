#include <time.h>
#include <sys/time.h>
#include <locale.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 200

int
main(int argc, char *argv[])
{
	time_t t;
	struct tm *loc;
	char buf[BUF_SIZE];

	if (setlocale(LC_ALL, "") == NULL)
		errExit("setlocale");     /* use locale settings in conversions */

  t = time(NULL);

  printf("ctime() of time() value is: %s", ctime(&t));

  loc = localtime(&t);
  if (loc == NULL)
    errExit("localtime");

  printf("asctime() of local time is: %s", asctime(loc));

  if (strftime(buf, BUF_SIZE, "%A, %d %B %Y, %H:%M:%S %Z", loc) == 0)
    fatal("strftime() of local time is: %s\n", buf);
  printf("strftime() of local time is: %s\n", buf);

  struct timeval tv;
  int ret;

  ret = gettimeofday(&tv, NULL);  /* 0 on success */
  if (ret)
    perror("gettimeofday");
  else
    printf("seconds=%ld useconds=%ld\n",
        (long) tv.tv_sec, (long) tv.tv_usec);



  exit(EXIT_SUCCESS);
}
