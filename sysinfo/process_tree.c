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
#include <sys/stat.h>		/* for stat macros */
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

struct _proc {
	pid_t pid;
	/* pid_t   pgid; */
	/* uid_t   uid; */
	PROC *parent;
	list_head *children;
	char name[LINE_MAX];	/* _PC_NAME_MAX instead? */
};

struct _list_head {
	PROC *process;
	list_head *next;
};

list_head *
list_init()
{
	list_head *head = malloc(sizeof (*head));
	if (head == NULL)
        errExit("failed to init list\n");

	head->next = head;

	return head;
}

list_head *
list_add(list_head *list, list_head *node)
{
    if (list == NULL) {
        list = list_init();
        list->process = node->process;
        return list;
    }

	list_head *temp = list->next;
	node->next = temp;
	list->next = node;
    return list;
}



void
print_list(list_head * list)
{
	list_head *p, *c;
    PROC *par;
	klist_for_each(p, list) {
		printf("%s: %zu\n", p->process->name, (long) p->process->pid);


        /* for (par = p->process->parent; par != NULL; par = par->parent) */
        /*     printf("->%zu-|%s", (long) p->process->parent->pid, p->process->parent->name); */
        /* printf("\n"); */


        if (p->process->children) {
            /* print_list(p->process->children); */
            klist_for_each(c, p->process->children) {
                printf("|-");
                printf("-%zu-|\t%s\n", (long) c->process->pid, c->process->name);
            }

            printf("\n");
        }
	}
}


void
free_list(list_head * list)
{
	list_head *p;
	klist_for_each(p, list) {
		printf("deleting: %zu %s\n", (long) p->process->pid,
		       p->process->name);

		if (p != p->next) {
			list_head *temp = p->next;
			p->next = p->next->next;
			free(temp);
		}
	}
}

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
 * @param [out] list points to match
 * @returns TRUE if match else FALSE
 */
static Boolean
list_find_pid_match (list_head **list_out, pid_t pid)
{
    list_head *list = *list_out;
    if (!list || !list->next)
        return FALSE;

    list_head *p;
    klist_for_each(p, list) {
        if (pid_matches(p, pid)) {
            *list_out = p;
            return TRUE;
        }
    }
    return FALSE;

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
proc_fetch_or_alloc(list_head *list, pid_t pid)
{
    list_head *p;
    klist_for_each(p, list) {
        if (pid_matches(p, pid)) {
            return p;
        }
    }

	/* no match found */
	list_head *new_node = malloc(sizeof (*new_node));

	new_node->process = malloc(sizeof (PROC));
	if (!new_node || !new_node->process)
		errExit("Failed to malloc node\n");

	/* insert node into chain */
    (void) list_add(list, new_node);
	return new_node;
}

list_head *
list_add_proc(list_head *list, PROC *process)
{
    list_head *node;
    if (!process->pid)
        errExit("No pid available\n");

    if (list_find_pid_match(&list, process->pid)) {
        return list;
    }

    node = malloc(sizeof(*node));
    node->process = process;

    list = list_add(list, node);
    return list;
}

static char *
get_path(pid_t pid)
{
    /* @TODO size path */
	static char path[BUFSIZ];
	sprintf(path, "%s/%zu/%s", PROC_BASE, (long) pid, PROC_STATUS);
    return path;
}

static void
read_proc_dir_by_pid(pid_t pid, list_head *list, list_head *child)
{
	FILE *file;
	char *path;
	char buf[LINE_MAX];
    pid_t ppid;

    path = get_path(pid);
    if (!path)
        return;

	if ((file = fopen(path, "r")) != NULL) {
        list_head *selected_process = proc_fetch_or_alloc(list, pid);
        if (child && child->process && child->process->pid) {
            child->process->parent = selected_process->process;
            child->process->parent->children = list_add_proc(child->process->parent->children, child->process);
        }


        PROC *proc = selected_process->process;
		while (fgets(buf, LINE_MAX, file)) {
			if (buf[strlen(buf) - 1] == '\n')	/* replace fgets \n with \0 */
				buf[strlen(buf) - 1] = '\0';

                if (str_starts_with(buf, "Name"))
                    sscanf(buf, "%*s%s", proc->name);

                if (str_starts_with(buf, "Pid"))
                    sscanf(buf, "%*[^]0-9]%d", &proc->pid);

                /* handle parent */
                if (str_starts_with(buf, "PPid")) {
                    sscanf(buf, "%*[^]0-9]%d", &ppid);
                    if (ppid > 1)
                        read_proc_dir_by_pid(ppid, selected_process, selected_process);
                }


		}
		fclose(file);
	}

}

/**
 * @brief crawl PROC_BASE and create PROC entries
 */
static void
crawl_proc(list_head * list_head)
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
		if (de == NULL)	/* EOS */
			break;

		pid = (pid_t) strtol(de->d_name, NULL, 10);
		if (pid)
			read_proc_dir_by_pid(pid, list_head, (void  *) NULL);
	}

	closedir(dirp);

}



int
main(int argc, char *argv[])
{
	/* create initial list element linked to itself */
	list_head *list_head = list_init();

	/* open current dir to return to later */
	int ret;

#ifdef ROOT_RUN
	int swd_fd;
	swd_fd = open(".", O_RDONLY);
	if (swd_fd == -1)
		errExit("Could not open current dir\n");

	/* change to root */
	ret = chdir("/");
	if (ret == -1) {
		errExit("Could not change to root dir\n");
	}
#endif

	/* create circular list of all unique running pids */
	crawl_proc(list_head);


	/* free_list(list_head); */
	print_list(list_head);

#ifdef ROOT_RUN
    /**
     * return to original dir. unneccessary as only process runs from the chdir.
     * here for example only.
     */
	ret = fchdir(swd_fd);
	if (ret == -1) {
		errExit("Returning to original dir\n");
	}

	/* close dir */
	ret = close(swd_fd);
	if (ret == -1) {
		errExit("Could not close original dir");
	}
#endif

	exit(EXIT_SUCCESS);
}
