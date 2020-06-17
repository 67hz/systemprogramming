#include <time.h>
#include "curr_time.h"

#define BUF_SIZE 1000

/**
 * @return string containing current time formatted according to spec
 * in 'format' (see strftime(3) for specifiers).
 * If 'format' is NULL, use "%c" (gives datae and time as for ctime(3), but
 * without trailing NL).
 * Return NULL on error
 */
char *
currTime(const char *format)
{
  static char buf[BUF_SIZE];  /* nonreentrant (not safe to call concurrently) */
  time_t t;
  size_t s;
  struct tm *tm;

  t = time(NULL);
  tm = localtime(&t);
  if (tm == NULL)
    return NULL;

  s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);

  return (s == 0) ? NULL : buf;

}
