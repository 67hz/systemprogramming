#define _DEFAULT_SOURCE /* Get getpass() declaration from <unistd.h> */
#if ! defined (_XOPEN_SOURCE) || _XOPEN_SOURCE < 500      /* get crypt() from <unistd.h> */
#define _XOPEN_SOURCE 500
#endif
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include "tlpi_hdr.h"

struct passwd *getpwnamMine(const char *name) {
  struct passwd *pwd;
  setpwent();
  while ((pwd = getpwent()) != NULL) {
    if (strcmp(pwd->pw_name, name) == 0)
      return pwd;
  }

  endpwent();

  return NULL;
}

int
main(int argc, char **argv)
{
	char *username, *password, *encrypted, *p;
	struct passwd *pwd;
  struct spwd *spwd;
  Boolean authOk;
  size_t len;
  long lnmax;
  
  lnmax = sysconf(_SC_LOGIN_NAME_MAX);
  if (lnmax == -1)   /* if limit is indeterminate */
    lnmax = 256;

  username = malloc(lnmax);
  if (username == NULL)
    errExit("malloc");

  printf("Username: ");
  fflush(stdout);
  if (fgets(username, lnmax, stdin) == NULL)
    exit(EXIT_FAILURE);   /* Exit on EOF */

  len = strlen(username);
  if (username[len - 1] == '\n')
    username[len-1] = '\0'; /* remove trailing 0 */

  /* using my implementation of getpwnam */
  pwd = getpwnam(username);
  if (pwd == NULL)
    fatal("Couldn't get password record");

  spwd = getspnam(username);
  if (spwd == NULL && errno == EACCES)
    fatal("No permission to read shadow password file");

  if (spwd != NULL)   /* there is a shadow pw record */
    pwd->pw_passwd = spwd->sp_pwdp; /* use the shadow pw */

  /* getpass obsolete */
  password = getpass("Password: ");

  /* encrypt pw and erase cleartext immed */
  encrypted = crypt(password, pwd->pw_passwd);
  for (p = password; *p != '\0'; )
    *p++ = '\0';

  if (encrypted == NULL)
    errExit("crypt");

  authOk = strcmp(encrypted, pwd->pw_passwd) == 0;
  if (!authOk) {
    printf("Incorrect password\n");
    exit(EXIT_FAILURE);
  }

  printf("Successfully authenticated: UID=%ld\n", (long) pwd->pw_uid);

  /* do authenticated work now */
  exit(EXIT_SUCCESS);

}
