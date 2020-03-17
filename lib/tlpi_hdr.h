#ifndef TLPI_HDR_H
#define TLPI_HDR_H

#include <sys/types.h>		/* type defns used by many programs */
#include <stdio.h>
#include <stdlib.h>		/* prototypes of commonly used lib functions,
				   plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>		/* prototypes for many system calls */
#include <errno.h>		/* declares errno and defines error constants */
#include <string.h>

#include "get_num.h"
#include "error_functions.h"

/* Some Unix implemenations have previous def of T/F */
#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif


typedef enum { FALSE, TRUE } Boolean;

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n))

/* some systems do not define 'socklen_t' */
#if defined(__sgi)
typedef int socklen_t;
#endif

#if defined(__sun)
#include <sys/file.h>       /* has def of FASYNC */
#endif

/** 
 * Further into the book, will need
 * more macro defn's at http://www.man7.org/tlpi/code/online/dist/lib/tlpi_hdr.h.html
 */





#endif				/* ifndef TLPI_HDR_H */
