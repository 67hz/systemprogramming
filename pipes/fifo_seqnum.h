#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define SERVER_FIFO "/tmp/seqnum_sv"
                                /* well known name for server's FIFO */

#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"
                                /* template for building client FIFO name */

#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)
                                /* Space required for client FIFO pathename
                                 * (+20 padding allowance for PID) */

struct request {        /* request (client --> server) */
    pid_t pid;          /* PID of client */
    int seqLen;         /* length of desired sequence */
};

struct response {       /* response (server --> client) */
    int seqNum;         /* start of sequence */
};
