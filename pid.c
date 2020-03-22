#include <stdio.h>
#include <unistd.h>
#include "lib/tlpi_hdr.h"

int main (int argc, char **argv)
{
  pid_t pid = getpid();
  pid_t ppid = getppid();

  printf("process pid: %d\n", pid);
  printf("process parent pid: %d\n", ppid);
}


