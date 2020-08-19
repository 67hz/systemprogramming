#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "read_line.h"

static ssize_t
my_read(int fd, char *ptr)
{
  static int read_cnt = 0;
  static char *read_ptr;
  static char read_buf[LINE_MAX];
}

ssize_t
readLine(int fd, void *buffer, size_t n)
{
  ssize_t numRead;  /* # bytes fetched from last read */
  size_t totRead;   /* toal bytes read so far */
  char *buf;
  char ch;

  if (n <= 0 || buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  buf = buffer;   /* no ptr arithmetic on "void *" */

  totRead = 0;
  for (;;) {
    numRead = read(fd, &ch, 1); /*
                                 * @FIXME reduce read() calls by creating
                                 * a buffer to read from. See Stevens 2.10
                                 */

    /* single char reader */
    if (numRead == -1) {
      if (errno == EINTR) /* interrupted so restart read */
        continue;

      else
        return -1;

    } else if (numRead == 0) {  /* EOF */
      if (totRead == 0)
        return 0;
      else
        break;                         /* some bytes read */

    } else {                          /* read 1 byte */
      if (totRead < n - 1) {
        totRead++;
        *buf++ = ch;
      }

      if (ch == '\n')   /* read until NL, NL is included above */
        break;
    }
  }

  *buf = '\0';          /* null terminate */
  return totRead;
}
