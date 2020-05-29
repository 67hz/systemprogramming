#ifndef _UGID_FUNCTIONS_H
#define _UGID_FUNCTIONS_H

#include <pwd.h>

/************************************************/
/*                                              */
/*--------------- Public Interface -------------*/
/*                                              */
/************************************************/
char * userNameFromId(uid_t uid);

uid_t userIdFromName(const char *name);

char * groupNameFromId(gid_t gid);

gid_t groupIdFromName(const char *name);




#endif /* ifndef _UGID_FUNCTIONS_H */
