/*
 * get_num.h
 *
 * Header file for get_num.h
 */

#ifndef GET_NUM_H
#define GET_NUM_H

#define GN_NONNEG       01      /* Values must be >= 0 */
#define GN_GT_0         02      /* Values must be > 0 */

                                /* By default, integers are decimal */
#define GN_ANY_BASE     0100    /* Can use any base - like strtol(3) */
#define GN_BASE_8       0200    /* Value expressed in octal */
#define GN_BASE_16      0400    /* Value expreesed in hex */

long getLong(const char *arg, int flags, const char *name);

int getInt(const char *arg, int flags, const char *name);


#endif /* ifndef GET_NUM_H */
