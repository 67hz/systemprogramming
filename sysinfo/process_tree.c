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
 *  take sum of pid digits??? 7 * 9 = max of 63 spaces (64 bits use bitmask???) *  [sum][num_digits]->SingleList for collisions
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

/* based on kernel */
#define klist_for_each(p, list) \
    for (p = list->next; p != list; p = p->next)

#define pid_matches(list_head, tid)   ( ((list_head)->process && (list_head)->process->pid == tid) ? 1 : 0)

#define list_is_tail(list, element) ((element)->next == NULL ? 1 : 0)

typedef struct _proc PROC;
typedef struct _list_head list_head;

struct _proc
{
    pid_t   pid;
    /* pid_t   pgid; */
    /* uid_t   uid; */
    PROC    *parent;
    list_head *children;
    char    name[LINE_MAX]; /* _PC_NAME_MAX instead? */
};

struct _list_head {
    PROC* process;
    list_head *next;
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
list_head *
proc_fetch_or_alloc(list_head *list, pid_t pid) {
    list_head *p;
    PROC *proc;
    klist_for_each(p, list) {
        if (pid_matches(p, pid)) {
            printf("got match\n");
            return p;
        }
    }

    /*
     * no match found 
     * will always have a p - inits to circular list
     * p = list if not found above
     */
    list_head *new_node = malloc(sizeof(*new_node));
    new_node->process = malloc(sizeof(PROC));
    if (!new_node || !new_node->process)
        errExit("Failed to malloc node\n");


    /* insert node into chain */
    list_head *temp = list->next;
    new_node->next = temp;
    list->next = new_node;


    return new_node;
}

static pid_t
process_fill_by_status (char *buf, list_head *node, pid_t pid)
{
    pid_t ppid;


    if (str_starts_with(buf, "Name")) {
        sscanf(buf, "%*s%s", node->process->name);
        return 0;
    }

    if (str_starts_with(buf, "Pid")) {
        sscanf(buf, "%*[^]0-9]%d", &node->process->pid);
        return 0;
    }


    if (str_starts_with(buf, "PPid")) {
        sscanf(buf, "%*[^]0-9]%d", &ppid);
        if (ppid > 0 && ppid != 1)
            return ppid;
    }

    return 0;
}

static void
read_proc_dir_by_pid (pid_t pid, list_head *process)
{
    FILE *file;
    char *path;
    char buf[LINE_MAX];
    int ret_pid;

    if (!(path = malloc(strlen(PROC_BASE) +
                    strlen(PROC_STATUS) + 10))) /* extra '/'s, pid, and NL */
        errExit("Could not get full path: %zu", (long) pid);

    sprintf(path, "%s/%zu/%s", PROC_BASE, (long) pid, PROC_STATUS);


    if ((file = fopen (path, "r")) != NULL) {
        /* printf("path: %s\n", path); */
        list_head *selected_process = proc_fetch_or_alloc(process, pid);

        while (fgets(buf, LINE_MAX, file))
        {
            if (buf[strlen(buf) - 1] == '\n')   /* replace fgets \n with \0 */
                buf[strlen(buf) - 1] = '\0';
            if ((ret_pid = process_fill_by_status(buf, selected_process, pid)) != 0)
                read_proc_dir_by_pid(ret_pid, process);
        }
        fclose(file);
    }

}

/**
 * @brief crawl PROC_BASE and create PROC entries
 */
static void
crawl_proc (list_head *list_head)
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

        pid = (pid_t) strtol(de->d_name, NULL, 10);
        if (pid)
            read_proc_dir_by_pid(pid, list_head);
    }

    closedir(dirp);

}

void
print_list(list_head *list)
{
    list_head *p;
    klist_for_each(p, list) {
        printf("Process: %zu - %s\n", (long) p->process->pid, p->process->name);
    }
}

list_head*
list_init ()
{
    list_head *head = malloc(sizeof(*head));
    if (head == NULL)
        errExit("list_init alloc");

    /* no process for initial element */

    /*
     * make this a circular list - makes traversing as simple as checking if 
     * element == start element. thus traversing can start from any element.
     */
    head->next = head;

    return head;
}


int main(int argc, char *argv[])
{
    /* create initial list element linked to itself */
    list_head* list_head = list_init();

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

    print_list(list_head);

    /*
     * @TODO free_list(list_head);
     */



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
