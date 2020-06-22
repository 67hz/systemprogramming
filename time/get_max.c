#include <limits.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "tlpi_hdr.h"


int main(int argc, char *argv[])
{
  long clockTicks = sysconf(_SC_CLK_TCK);   /* # clock ticks per second */
  printf("max int value:%d\n", INT_MAX);
  printf("CLOCKS_PER_SEC=%ld sysconf(_SC_CLK_TCK)=%ld\n",
      (long) CLOCKS_PER_SEC, clockTicks);
  printf("%.2f seconds before cycle restarts in time() calls\n", (double) INT_MAX / sysconf(_SC_CLK_TCK));
  printf("%.2f seconds before CLOCKS_PER_SEC cycle restarts in clock() calls\n", (double) INT_MAX / CLOCKS_PER_SEC);

  
  return 0;
}
