Processes
=========

An instance of an executing program. Abstract entity defined by kernel where system resources are allocated in order to execute a program.

Kernel's POV of process:
* user-space  memory containing program code, variables used by code, range of kernel data structures to maintain info about state of process
    * DS info: IDs associated with process, virtual memory tables, table of open fds, info relating to signal delivery and handling, process resource usages and limits, CWD, ...

Program: file containing a range of information describing how to construct a process at runtime
	- Binary format id: a.out (assembler output), COFF (Common Object File Format), now mostly ELF (Executable and Linkable Format)
	- Machine language instructions
 	- Program entry-point address: where execution of program starts
	- Data
	- Symbol and relocation tables: functions, vars. used for debugging and runtime symbol resolution (dynamic linking)
	- Shared library and dynamic-linking info: fields listing shared library dependencies and pathname of dynamic linker to load deps
	- Other info: how to construct a process





