#define _DEFAULT_SOURCE
#include <time.h>
#include <locale.h>
#include "tlpi_hdr.h"

#define SBUF_SIZE 1000

int
main(int argc, char *argv[])
{
	struct tm tm;
	char sbuf[SBUF_SIZE];
	char *ofmt;

	if (argc < 3 || strcmp(argv[1], "--help") == 0)
		usageErr("%s input date-time in-format [out-format]\n", argv[0]);

  if (setlocale(LC_ALL, "") == NULL)
    errExit("setlocale");   /* use locale settings in conversions" */

  /* tm = (struct tm){}; */
  memset(&tm, 0, sizeof(struct tm));   /* init tm */
  if (strptime(argv[1], argv[2], &tm) == NULL)
    fatal("strptime");

  tm.tm_isdst = -1;   /* not set by strptime(); tells mktime()
                         to determine if DST is in effect */
  printf("calendar time (seconds since EPOCH): %ld\n", (long) mktime(&tm));

  ofmt = (argc > 3) ? argv[3] : "%H:%M:%S %A, %d %B %Y %Z";
  if (strftime(sbuf, SBUF_SIZE, ofmt, &tm) == 0)
    fatal("strftime returned 0");
  printf("strftime() yields: %s\n", sbuf);

  exit(EXIT_SUCCESS);
}
