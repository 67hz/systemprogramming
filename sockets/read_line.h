#include <sys/types.h>
/**
 *
 * @return number of bytes copied into buffer (excl. terminating null byte), or
 * 0 on EOF, or -1 on error
 * 
 */
ssize_t readLine(int fd, void *buffer, size_t n);
