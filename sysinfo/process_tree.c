/**
 * @brief List all processes as a tree going back to init.
 *
 * Username is not required, could filter for only processes for the user ITF.
 *
 *
 *  All pid stored in global pid arr [< pid_max] where pid hashed
 *  pointing to a PROC?
 *
 *  hash ideas:
 *  take sum of pid digits??? 7 * 9 = max of 63 spaces (64 bits use bitmask???)
 *  [sum][num_digits]->SingleList for collisions
 *  * see kernel's pidhash
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
#include "error_functions.h"
#include "tlpi_hdr.h"
#include "ugid_functions.h"


#ifdef ROOT_RUN
#define PROC_BASE           "proc"
#else
#define PROC_BASE           "/proc"
#endif

#define PROC_STATUS         "status"

/* see LKD p.30
   plist similar to kernel's task_list of task_struct(s)
   or /linux/pid.h for foreach macros */

#define for_each(list_head)      \
    if (list_head)               \
        for (; (list_head)->next != NULL; (list_head) = (list_head)->next)

#define pid_matches(list_head, pid)   ( ((list_head)->proc && (list_head)->proc->pid == pid) ? 1 : 0)
#define list_is_tail(list, element) ((element)->next == NULL ? 1 : 0)

typedef struct _proc PROC;
typedef struct _child CHILD;

struct _proc
{
    pid_t   pid;
    /* pid_t   pgid; */
    /* uid_t   uid; */
    PROC    *parent;
    CHILD   *children;
    char    name[LINE_MAX]; /* _PC_NAME_MAX instead? */
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


/**
 * @brief return *CHILD by pid or allocate space for new CHILD
 * @return *CHILD from task_list if exists else allocate new *CHILD.
 *
 * This will search the task list (linear search a single-linked list) for
 * matching pid. If found, returns *CHILD otherwise allocate space for new
 * CHILD.
 *
 * @remarks Align memory or allocate enough space for all CHILD's at once?
 */
CHILD*
proc_fetch_or_alloc (CHILD *list_head, pid_t pid)
{
    for_each(list_head) {
        if (pid_matches(list_head, pid))
            return list_head;
    }

    /* otherwise allocate new CHILD */
    if (!list_head)
        list_head = malloc(sizeof(*list_head));

    /* @TODO clean up nested structure allocations */
    if (!list_head->proc)
        list_head->proc = malloc(sizeof(PROC));


    if (list_head == NULL)
        errExit("Failed to malloc PROC");

    return list_head;
}

static int
process_fill_by_status (char *buf, CHILD *list_head, pid_t pid)
{
    pid_t ppid;
    CHILD *child = list_head;

    if (str_starts_with(buf, "Name")) {
        /* printf("%s", buf); */
        /* sscanf(buf, "%s%s", field, proc->name); */
        sscanf(buf, "%*s%s", child->proc->name);
        printf("\nProcess: %s", child->proc->name);
    }
    if (str_starts_with(buf, "Pid")) {
        sscanf(buf, "%*[^]0-9]%d", &child->proc->pid);
        printf("\nPID: %d\n", child->proc->pid);
    }


    if (str_starts_with(buf, "PPid")) {
        sscanf(buf, "%*[^]0-9]%d", &ppid);
        printf("\nPPid: %zu\n", (long) ppid);
        /* PROC *parent_proc = proc_fetch_or_alloc(child->proc->parent, ppid); */
    }


    /* @TODO add Uid, Gid */
    /* free(proc); */

    return 0;
}

static void
read_proc_dir (struct dirent *de, CHILD *list_head)
{
    FILE *file;
    char *path;
    char buf[LINE_MAX];
    pid_t pid;
    pid = (pid_t) strtol(de->d_name, NULL, 10);
    if (pid) {  /* only want numbered  dirs */


        if (!(path = malloc(strlen(PROC_BASE) + strlen(de->d_name) +
                        strlen(PROC_STATUS) + 3))) /* extra '/'s and NL */
            errExit("Could not get full path: %s", de->d_name);

        sprintf(path, "%s/%d/%s", PROC_BASE, pid, PROC_STATUS);


        if ((file = fopen (path, "r")) != NULL) {
            printf("path: %s\n", path);
            CHILD *child_process = proc_fetch_or_alloc(list_head, pid);

            while (fgets(buf, LINE_MAX, file))
            {
                if (buf[strlen(buf) - 1] == '\n')   /* replace fgets \n with \0 */
                    buf[strlen(buf) - 1] = '\0';
                process_fill_by_status(buf, child_process, pid);
            }
            fclose(file);
        }


    }
}

/**
 * @brief crawl PROC_BASE and create PROC entries
 */
static void
crawl_proc (CHILD *list_head)
{
    DIR *dirp;
    struct dirent *de;

    dirp = opendir(PROC_BASE);
    if (dirp == NULL) {
        errExit("opendir");
    }

    /* iterate over dirs in PROC_BASE */
    for (;;) {
        errno = 0;
        pid_t pid;

        de = readdir(dirp);
        if (de == NULL) /* EOS */
            break;

        read_proc_dir(de, list_head);
    }

    closedir(dirp);

}


int main(int argc, char *argv[])
{
    PROC *current;
    CHILD *list_head = proc_fetch_or_alloc(list_head, 0);
    list_head->proc = current;

    /* open current dir to return to later */
    int ret;

#ifdef ROOT_RUN
    int swd_fd;
    swd_fd = open (".", O_RDONLY);
    if (swd_fd == -1)
        errExit("Could not open current dir\n");

    /* change to root */
    ret = chdir ("/");
    if (ret == -1) {
        errExit("Could not change to root dir\n");
    }
#endif

    crawl_proc(list_head);


#ifdef ROOT_RUN
    /**
     * return to original dir. unneccessary as only process runs from the chdir.
     * here for example only.
     */
    ret = fchdir (swd_fd);
    if (ret == -1) {
        errExit("Returning to original dir\n");
    }

    /* close dir */
    ret = close (swd_fd);
    if (ret == -1) {
        errExit("Could not close original dir");
    }
#endif

    
    exit(EXIT_SUCCESS);
}
