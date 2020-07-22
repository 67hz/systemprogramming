#include <asm-generic/errno-base.h>
#include <fcntl.h>
#include <signal.h>
#include "fifo_seqnum.h"


int main(int argc, char *argv[])
{
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0; /* this is our "service" */

    /* create a well known FIFO, and open it for reading */

    umask(0);   /* to get the permissions we want */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1
            && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);

    if ( (serverFd = open(SERVER_FIFO, O_RDONLY) == -1))
        errExit("open %s", SERVER_FIFO);


    /* open extra write descriptor to avoid EOF if no clients or all clients
     * close FD's */

    if ( (dummyFd = open(SERVER_FIFO, O_RDONLY) == -1))
        errExit("open %s", SERVER_FIFO);

    /* ignore SIGPIPE in case server writes to client FIFO that does not have
     * a reader. Instead of SIGPIPE signal (kills process by default), it
     * will receive an EPIPE error from the write() system call
     */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");

    for (;;) {                                    /* read reqs and send resps */
        if (read(serverFd, &req, sizeof(struct request))
                != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;
        }

        /* open client FIFO (prev created by client */

        /* str, size, format */
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                (long) req.pid);
        if ( (clientFd = open(clientFifo, O_WRONLY) == -1)) {
            errMsg("open %s", clientFifo); /* open failed, give up on client */
            continue;
        }

        /* send response and close FIFO */

        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response))
                != sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        if (close(clientFd) == -1)
            errMsg("close");

        seqNum += req.seqLen;       /* update sequence number */
    }







    exit (EXIT_SUCCESS);
}
