
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include "tlpi_hdr.h"
#include "ugid_functions.h"


/*
 * use feof to check if EOF on fp
 * int feof(FILE *stream);
 */

#define MAX_NAME  100

/**
 * this seems overkill. is there a better way to avoid white space
 * in fgets?
 */
void
trimTrailingWhiteSpace(char *str)
{
  size_t i = 0;
  size_t index = -1;
  while (str[i] != '\0') {
    if (str[i] != ' ' && str[i] != '\n')
      index = i;

    i++;
  }
  str[index + 1]  = '\0';
}

int main(int argc, char *argv[])
{
  uid_t userId;
  gid_t groupId;
  char *fetchedName;
  char *groupName;
  char *name;
  char runName[MAX_NAME];

  if (argc < 2) {
    FILE *fpipe;

    if ((fpipe = popen("whoami", "r")) == 0)
      errExit("Could not determine user");

    if (fpipe == NULL)
      errExit("popen");

    fgets(runName, MAX_NAME, fpipe);

    name = strdup(runName);
    trimTrailingWhiteSpace(name);

    pclose(fpipe);
  } else
    name = argv[1];

  userId = userIdFromName(name);

  if (userId == -1)
    errExit("No user id found");

  fetchedName = userNameFromId(userId);
  groupId = groupIdFromName(fetchedName);
  groupName = groupNameFromId(groupId);

  char copiedName[6];
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
