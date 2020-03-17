/*
 * determining glibc version
 */

#include <stdio.h>
#include <stdlib.h>
#include <gnu/libc-version.h>
#include <unistd.h>

int
main ()
{
  /* option 1 */
  const char *version = gnu_get_libc_version ();
  printf("\nVersion: %s\n", version);


  /* option 2 */
  char *glibcbuf;
  size_t n;
  n = confstr (_CS_GNU_LIBC_VERSION, NULL, (size_t) 0);
  glibcbuf = malloc (n);
  if (glibcbuf == NULL)
    abort();
  confstr (_CS_GNU_LIBC_VERSION, glibcbuf, n);
  printf ("Version: %s", glibcbuf);


  /* test atoi */
  char *string = "2343242  1111and me";
  printf("\nvalue: %d", atoi(string));
}
