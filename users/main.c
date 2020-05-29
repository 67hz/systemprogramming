
#include <stdio.h>
#include "ugid_functions.h"


int main(int argc, char *argv[])
{
  uid_t userId;
  const char* name = "aaron";

  userId = userIdFromName(name);
  
  printf("User Id from %s: %d\n", name, userId);
  return 0;
}
