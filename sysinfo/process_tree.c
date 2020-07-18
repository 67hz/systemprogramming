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
 *   see kernel's pidhash
 *
 *   m = proc/sys/kernel/pid_max (>= v2.6 so set default if not defined)
 *   m is subject to change as processes added/removed
 *
 *  * fork process to create buffer and hash table in parallel
 *
 *   clean up: strtol error handling
 *   get_pid_max : fall back for pre 2.6 Linux
 *   switch to gnome-docs
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

#ifdef ROOT_RUN
#define PROC_BASE           "proc"
#else
#define PROC_BASE           "/proc"
#endif

#define MAX_LINE 100        /* for small buffers */

/**
 * set the 'm' for the pid hash table
 */
#ifndef PIDHASH_SZ
#define PIDHASH_SZ 16
#endif

#define PROC_STATUS         "status"

/* see LKD p.30
   plist similar to kernel's task_list of task_struct(s)
   or /linux/pid.h for foreach macros */

/* based on kernel */
#define klist_for_each(p, list) \
    for ((p) = (list)->next; (p) != (list); (p) = (p)->next)

#define list_head_pid(list_head) (((list_head) && (list_head)->process && (list_head)->process->pid) ? (list_head)->process->pid : -1)

#define pid_matches(list_head, tid)   (((list_head) && (list_head)->process && (list_head)->process->pid == (tid)) ? 1 : 0)

#define list_is_tail(list, element) ((element)->next == NULL ? 1 : 0)

typedef struct _proc PROC;
typedef struct _list_head list_head;

struct _proc {
	pid_t pid;
	pid_t   ppid;
	/* uid_t   uid; */
	PROC *parent;
	list_head *children;
	char name[LINE_MAX];	/* _PC_NAME_MAX instead? */
};

struct _list_head {
	PROC *process;
	list_head *next;
};

typedef list_head** hash_t;     /* less **s at the sake of misdirection :) */

static int
get_pid_max ()
{
#ifndef PIDHASH_SZ          /*
                             * short-circuit for now
                             * current pid_max in kernel is > 1 mil
                             */

    int fd;
    char line[MAX_LINE];
    int pid_max;

    if ((fd = open("/proc/sys/kernel/pid_max", O_RDONLY)) == -1)
        return -1;

    ssize_t n;
    if ((n = read(fd, line, MAX_LINE)) == -1)
        return -1;

    /* @TODO pass * instead of NULL and check value */
    if ((pid_max = (pid_t) strtol(line, NULL, 10) == 0))
        return -1;

    return pid_max;
#endif
    return PIDHASH_SZ;
}

/**
 * @brief hash a pid - from kernel 2.4
 *
 * @return int key
 * @remarks m must be a power of 2
 * (x & (a - 1)) is same as x mod a iff is power of 2
 *
 */
static inline
int
hash_pid (pid_t pid)
{
    return ((pid >> 8) ^ pid) & (PIDHASH_SZ - 1);
}

list_head *
list_init() {
  list_head *head = calloc(1, sizeof(*head));
  memset(head, 0, sizeof(*head));
  if (head == NULL)
    errExit("failed to init list\n");

    head->next = head;

    return head;
}

list_head *
list_add(list_head **list, list_head *node)
{
    if (*list == NULL) {
      *list = list_init();
      (*list)->process = node->process;
      return *list;
    }

    /* check if already in list by pid */
    list_head *iter;

    klist_for_each(iter, *list)
        if (pid_matches(iter, list_head_pid(node)))
            return iter;

	iter = (*list)->next;
	node->next = iter;
	(*list)->next = node;
    return *list;
}


void
print_hash_list(const hash_t hash_table)
{
    int i = 0;
    int buckets = get_pid_max();
    list_head *list;

    for (i = 0; i < buckets; i++) {
        list = hash_table[i];
        printf("\n\n\nBUCKETS: \t %d\n", i);

        for (; list->next != NULL; list = list->next) {
            printf("%s: %zu\n", list->process->name, (long) list->process->pid);
            printf("|\n");
        }
    }
}

void
print_list(const list_head * list)
{
	list_head *p, *c;
    PROC *par;
	klist_for_each(p, list) {
		printf("%s: %zu\n", p->process->name, (long) p->process->pid);
        printf("|\n");


        list_head *children = p->process && p->process->children ? p->process->children : NULL;
        while (children) {
            klist_for_each(c, children) {
                printf("|-%s-|%zu\n", c->process->name, (long) c->process->pid);
            }
            children = c->process && c->process->children ? c->process->children : NULL;
            printf("|\n");
        }
	}
}

/**
 * Remove child from list and return child.
 * @param list to remove element from
 * @param node to remove
 * @return @node
 * @remark This does not free child and will return node regardless of it being
 * in the list (unlikely).
 */
list_head *list_remove_and_return(list_head *list, list_head *node) {
  list_head *iter = list->next;
  list_head *prev = NULL;

  while (iter && iter != list) {
    prev = iter;
    iter = iter->next;
    if (pid_matches(node, list_head_pid(iter))) {

      /* remove node from list (don't free) and return it */
      prev->next = iter->next;
      return iter;
    }
  }

  /* if match not found, return original node */
  return node;
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
	new_node->process = calloc(1, sizeof (PROC));


	if (!new_node) //|| !new_node->process)
		errExit("Failed to malloc node\n");

	/* insert node into chain */
    (void) list_add(&list, new_node);
	return new_node;
}

static char *
get_path(pid_t pid)
{
    /* @TODO size path */
	static char path[BUFSIZ];
	sprintf(path, "%s/%zu/%s", PROC_BASE, (long) pid, PROC_STATUS);
    return path;
}

/**
 * @brief Init a chained hash table of * to list_head
 * @param [out] hash_table ptr to hash_table
 * @return 0 on success, -1 on error
 */
int
hash_init (hash_t hash_table)
{
    int i;
    int pid_max = get_pid_max();

    for (i = 0; i < pid_max; i++) {
        list_head *new_list = list_init();
        new_list->next = NULL;  /* singly linked list for hash */
        hash_table[i] = new_list;
    }

    return 0;
}

/**
 *
 * @param hash_table of list_head ptrs
 * @param pid
 * @param [out] data if item found, data is passed here
 *
 * @return 0 if found, else -1
 */
static
int
hash_lookup (hash_t hash_table, pid_t pid, list_head *data)
{
    list_head *iter;
    int key = hash_pid(pid);

        klist_for_each(iter, hash_table[key])
            if (pid_matches(iter, pid)) {
                data = iter;
                return 0;
            }

    return -1;
}

static void
hash_add_process (hash_t hash_table, PROC const * const proc)
{
    list_head *data;
    int key = hash_pid ((proc)->pid);


    /* do nothing if h_table already has node (by pid) */
    /* if (hash_lookup (hash_table, proc->pid, data) == 1) */
    /*     return; */

    /* insert at head of list */
    list_head *node = calloc(1, sizeof(*node));
    node->process = (PROC *) proc;

    list_head *head = hash_table[key];
    node->next = head;
    hash_table[key] = node;

}

static void
read_proc_dir_by_pid(pid_t pid, list_head *list, hash_t hash_table)
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

        PROC *proc = selected_process->process;
		while (fgets(buf, LINE_MAX, file)) {
			if (buf[strlen(buf) - 1] == '\n')	/* replace fgets \n with \0 */
				buf[strlen(buf) - 1] = '\0';

                if (str_starts_with(buf, "Name"))
                    sscanf(buf, "%*s%s", proc->name);

                if (str_starts_with(buf, "Pid"))
                    sscanf(buf, "%*[^]0-9]%d", &proc->pid);

                /* handle parent */
                if (str_starts_with(buf, "PPid"))
                    sscanf(buf, "%*[^]0-9]%d", &proc->ppid);
		}
		fclose(file);
        hash_add_process(hash_table, proc);
	}


}

/**
 * @brief crawl PROC_BASE and create PROC entries
 */
static void
crawl_proc(list_head * list_head, hash_t hash_table)
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
			read_proc_dir_by_pid(pid, list_head, hash_table);
	}

	closedir(dirp);

}



int
main(int argc, char *argv[])
{
    hash_t hash_table;   /* hash_t is array of list_head *'s */
    if ((hash_table = (hash_t) calloc(get_pid_max(), sizeof(list_head *))) == NULL)
        errExit("Could not allocate hash table\n");

    hash_init(hash_table);

    printf("PID_MAX: %d\n", get_pid_max());

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
	crawl_proc(list_head, hash_table);

	/* free_list(list_head); */
	/* print_list(list_head); */
	print_hash_list(hash_table);

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
