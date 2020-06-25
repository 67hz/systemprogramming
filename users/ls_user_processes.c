/**
 * List all processes being run by user.
 *
 * readdir or nftw for traversal???
 * use stat to check for user id of owner???
 * read contents of /proc/pid/status to get name? not available in stat
 * get line starting with Uid:
 * if matches current process Uid, (save)/display result
 *
 * see strstr for searching file if not using stat
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "tlpi_hdr.h"



static void
listFiles(const char *dirpath)
{
}



int main
(int argc, char *argv[])
{



  
    exit(EXIT_SUCCESS);
}
