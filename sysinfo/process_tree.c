/**
 * @brief List all processes as a tree going back to init.
 *
 * Username is not required, could filter for only processes for the user ITF.
 *
 *
 *  All pid stored in global pid arr [pid_max] where pid is index
 *  pointing to a PROC? Could be a huge sparse array.
 *
 */


#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>   /* for stat macros */
#include "tlpi_hdr.h"
#include "ugid_functions.h"


#define PROC_BASE           "proc"
#define PROC_STATUS         "status"

typedef struct _proc PROC;
typedef struct _child CHILD;

struct _proc
{
    pid_t   pid;
    pid_t   pgid;
    uid_t   uid;
    char    name[_PC_NAME_MAX];
    CHILD   *children;
    PROC    *parent;
    PROC    *next;
};

struct _child {
    PROC    *proc;
    CHILD   *next;
};

static Boolean
str_starts_with(const char *restrict str, const char *restrict prefix)
{
    while (*prefix) {
        if (*prefix++ != *str++)
            return 0;
    }
    return 1;
}

static int
process_fill_by_status (char *buf, PROC *proc)
{
    /* char field[LINE_MAX]; */
    pid_t ppid;

    if (str_starts_with(buf, "Name")) {
        /* printf("%s", buf); */
        /* sscanf(buf, "%s%s", field, proc->name); */
        sscanf(buf, "%*s%s", proc->name);
        printf("\nProcess: %s", proc->name);
    }
    if (str_starts_with(buf, "Pid")) {
        sscanf(buf, "%*[^]0-9]%d", &proc->pid);
        printf("\nPID: %d\n", proc->pid);
    }

    /* @TODO if PPID exists and !=1 will need to traverse up the chain */
    /* if (str_starts_with(buf, "PPid")) { */
    /*     sscanf(buf, "%*[^]0-9]%d", &ppid); */
    /* } */


    /* @TODO add Uid, Gid */

    return 0;
}

/**
 * @brief crawl PROC_BASE and create PROC entries
 */
static void
crawl_proc ()
{
    DIR *dirp;
    struct dirent *de;
    FILE *file;
    char *path;
    char buf[LINE_MAX];

    dirp = opendir(PROC_BASE);
    if (dirp == NULL) {
        errExit("opendir");
    }

    /* iterate over dirs in PROC_BASE */
    for (;;) {
        errno = 0;
        int pid;

        de = readdir(dirp);

        if (de == NULL) /* EOS */
            break;

        pid = strtol(de->d_name, NULL, 10);
        if (pid) {  /* only want numbered  dirs */

            if (!(path = malloc(strlen(PROC_BASE) + strlen(de->d_name) +
                            strlen(PROC_STATUS) + 3))) /* extra '/'s and NL */
                errExit("Could not get full path: %s", de->d_name);

            sprintf(path, "%s/%d/%s", PROC_BASE, pid, PROC_STATUS);

            PROC *proc;
            if (! (proc = malloc(sizeof(*proc))))
                errExit("Could not allocate PROC for %d", pid);

            if ((file = fopen (path, "r")) != NULL) {
                while (fgets(buf, LINE_MAX, file))
                {
                    buf[strlen(buf) - 1] = '\0'; /* replace fgets \n with \0*/                 
                    process_fill_by_status(buf, proc);

                }
            }


        }
    }
    



}


int main(int argc, char *argv[])
{
    PROC *current;

    /* open current dir to return to later */
    int ret;
    int swd_fd;
    swd_fd = open (".", O_RDONLY);
    if (swd_fd == -1)
        errExit("Could not open current dir\n");

    /* change to root */
    ret = chdir ("/");
    if (ret) {
        errExit("Could not change to root dir\n");
    }

    crawl_proc();


    /**
     * return to original dir. unneccessary as only process runs from the chdir.
     * here for example only.
     */
    ret = fchdir (swd_fd);
    if (ret) {
        errExit("Returning to original dir\n");
    }

    /* close dir */
    ret = close (swd_fd);
    if (ret) {
        errExit("Could not close original dir");
    }


    
    exit(EXIT_SUCCESS);
}
