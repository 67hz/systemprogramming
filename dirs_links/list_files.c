#include <dirent.h>
#include "tlpi_hdr.h"

static void   /* list all files in 'dirpath' */
listFiles(const char *dirpath)
{
  DIR *dirp;
  struct dirent *dp;
  Boolean isCurrent;   

  isCurrent = strcmp(dirpath, ".") == 0;

  dirp = opendir(dirpath);
  if (dirp == NULL) {
    errMsg("opendir failed on '%s'", dirpath);
    return;
  }

  /* for each entry in this dir, print dir + filename */

  for (;;) {
    errno = 0;    /* to distinguish error from end of dir */
    dp = readdir(dirp);
    if (dp == NULL)
      break;

    /* skip '.' and '..' */
    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
      continue;

    if (!isCurrent)
      printf("%s/", dirpath);
    printf("%s\n", dp->d_name);
  }

  if (errno != 0)
    errExit("readdir");

  if (closedir(dirp) == -1)
    errMsg("closedir");
}

int
main(int argc, char *argv[])
{
  if (argc > 1 && strcmp(argv[1], "--help") == 0)
    usageErr("%s [dir...]\n", argv[0]);

  if (argc == 1)  /* no args, use CWD */
    listFiles(".");
  else
    for (argv++; *argv; argv++)
      listFiles(*argv);

  exit(EXIT_SUCCESS);
}
