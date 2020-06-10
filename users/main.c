
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include "ugid_functions.h"


int main(int argc, char *argv[])
{
  uid_t userId;
  gid_t groupId;
  char *fetchedName;
  char *groupName;
  const char *name = "aaron";
  char copiedName[6];

  userId = userIdFromName(name);
  fetchedName = userNameFromId(userId);
  groupId = groupIdFromName(fetchedName);
  groupName = groupNameFromId(groupId);

  memcpy(copiedName, fetchedName, 5);
  
  printf("User Id from %s: %d\n", name, userId);
  printf("Fetched name from id: %s\n", copiedName);
  printf("groupId: %d\ngroupName: %s\n", groupId, groupName);

  struct passwd *pwd;
  printf("Walking through /etc/passwd");

  while ((pwd = getpwent()) != NULL)
    printf("%-16s %5ld\n", pwd->pw_name, (long) pwd->pw_uid);

  endpwent();



  return 0;
}
