
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include "tlpi_hdr.h"
#include "ugid_functions.h"

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
  char runName[LINE_MAX];

  if (argc < 2) {
    FILE *fpipe;

    if ((fpipe = popen("whoami", "r")) == 0)
      errExit("Could not determine user");

    if (fpipe == NULL)
      errExit("popen");

    fgets(runName, LINE_MAX, fpipe);

    trimTrailingWhiteSpace(runName);
    name = strdup(runName);

    pclose(fpipe);

  } else
    name = argv[1];

  userId = userIdFromName(name);

  if (userId == -1)
    errExit("No user id found");

  fetchedName = userNameFromId(userId);
  groupId = groupIdFromName(fetchedName);
  groupName = groupNameFromId(groupId);

  char copiedName[strlen(name)];
  memcpy(copiedName, fetchedName, strlen(name));
  
  printf("User Id from %s: %d\n", name, userId);
  printf("Fetched name from id: %s\n", copiedName);
  printf("groupId: %d\ngroupName: %s\n", groupId, groupName);

  struct passwd *pwd;
  printf("Walking through /etc/passwd");

  while ((pwd = getpwent()) != NULL)
    printf("%-16s %5ld\n", pwd->pw_name, (long) pwd->pw_uid);

  endpwent();

  exit(EXIT_SUCCESS);
}
