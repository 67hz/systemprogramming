/* error_functions.h
 * 
 * Header file for error_functions.c
 */


#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

/**
 * print message on standard error
 * print error text corresponding to current value of errno: error name +
 * error description returned by strerror() followed by formatted output
 * specified in argument list.
 * @params same as printf, but with terminating `\n` appended to output
 */
void errMsg(const char *format, ...);

#ifdef __GNUC__

/**
 * stop 'gcc -Wall' complaining that "control reaches end of non-void function"
 * if the following functions terminate main() or another non-void function
 */

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

/**
 * operates like errMsg() but also terminates program by calling exit() or if
 * ENV var EF_DUMPCORE is defined with a non-empty string, calling abort() to
 * produce a core dump file for use with debugger.
 */
void errExit(const char *format, ...) NORETURN;

/**
 * like errExit but does not flush stdout before printing error message and
 * terminates process by call _exit() instead of exit() - causes process
 * to terminate without flushing stdio buffers or invoking exit handlers.
 */
void err_exit(const char *format, ...) NORETURN;

/**
 * same as err_exit but print errno (EN) - will be useful in programs that
 * emply the POSIX threads API - POSIX return non-negative errno and 0
 * on success.
 */
void errExitEN(int errnum, const char *format, ...) NORETURN;

/**
 * used to diagnose general errors, including from libs that don't set errno
 * prints formatted output on standard error and terminates
 * program like errExit()
 * @params same as printf and appends '\n' 
 */
void fatal(const char *format, ...) NORETURN;

/**
 * diagnose errors in command-line argument usage.
 * @params style of printf() prints string "Usage: " + formatted output on
 * standard error - terminates via exit()
 */
void usageErr(const char *format, ...) NORETURN;

/**
 * similar to usageErr() - used to diagnose errors in cmd line args
 */
void cmdLineErr(const char *format, ...) NORETURN;

#endif				/* ifndef ERROR_FUNCTIONS_H */
