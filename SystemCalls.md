System Calls
------------

* changes user state from user mode to kernel mode - so CPU can access protected kernel memory

* fixed set - each call id'd by unique #
  * programs identify syscalls by name (id schem is not usually visible to programs)

* may have args specifying info to be transferred from user space (process's virtual address space) to kernel space and vice-versa

* all sys calls enter kernel in same way
  * kernel must identify syscall - wrapper function copies sys call # into specific CPU register (%eax: lower 32 bits of rax)

* wrapper function executes a ***trap*** machine instruction (int 0x80) - processor switches form user to kernel mode and executes code pointed to by location 0x80 (128 decimal) of system's trap vector
    * sysenter is supported after 2.6 kernel from glibc 2.3.2 onward

  * in response to trap to location 0x80 - kernel invokes system_call()) routine (located in assembler file arch/i386/entry.s) to handletrap. Handler:
  * saves register values onto kernel stack
  * checks validity of system call number
  * invokes appropriate system call service routine (id'd against sys_call_table)
      * if any args, checks validity of args (e.g., addresses point to valid locations in user memory)
  * service routing performs req'd task: e.g., mod'ing values at addresses specified in args and transferring data between user memory and kernel memory (e.g., in I/O ops)
  * returns a result satus to system_call() routine

  * if return value is error, wrapper function sets global `errno` with value
  * wrapper funtion returns to caller with integer return value indicating success or failure
      * if error, C lib wrapper negates to make it positive and returns -1 as function result to indicate error


Unfallible syscalls
-------------------

No need to check return values from these. always check others.

* `getpid()` always returns ID of process
* `_exit()` always terminates a process


Checking return values
----------------------

```c
if (close(fd) == -1) {
  /* code to handle error */
}
```

On failure, `errno` <errno.h> set to positive value to id error.
* all symbolic names start with `E` (e.g., EINTR: interrupted by signal)

```c
if (cnt == -1) { // always check if return is error
  if (errno == EINTR) // then examine errno
    // fprintf (stderr, "read interrupted by a signal\n");
    // or use perror(msg) or strerror() - locale sensitive
    perror ("cnt");
    exit (EXIT_FAILURE);
  else
    /* some other error occured */
}
```

* successful syscalls never reset errno to 0. if errno is a value, it's from a previous failed call.


System Data Types 3.6.2
=======================

* various implementation data types are rep'd using standard C types: 
  * process IDs, user IDs, file offsets
      * PID canbe long on one system, long on another
  * instead of using int/long which can vary across platforms
  * SUSv3 specifies standard system data types for portability
      * pid_t = for proces IDs

          ```c
          typedef int pid_t;
          ```

* most standard system types end in `_t` and declared in <sys/types.h>
* apps should use these types to portably declare the variables it uses

      ```c
      pid_t mypid;
      ```
## print_f casting

use %ld with a long cast to normalize ints and longs from system data types

```c
pid_t mypid;
mypid = getpid();
printf("My PID is %ld\n", (long) mypid);
```


## system structs

Be speciific, order and extra fields are not guaranteed across platforms

```c
struct sembuf {
  unsigned short sem_num;
  short          sem_op;
  short          sem_flg;
};

struct sembuf s = {3. -1, SEM_UNDO}; /* WRONG */

/* RIGHT */
struct sembuf s;

s.sem_num = 3;
s.sem_op = -1;
s.sem_fig = SEM_UNDO;

/* RIGHT if C99 */
struct sembuf s = { .sem_num = 3, .sem_op = -1, .sem_flg = SEM_UNDO };
```

**For portability, include <sys/types.h> first**


