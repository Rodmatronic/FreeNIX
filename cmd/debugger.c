#include "../include/errno.h"
#include "../include/user.h"
#include "../include/stat.h"
#include "../include/fcntl.h"

char *argv[] = { "sh", 0 };

int
main()
{
  unsigned long time_t = 0;
  struct utsname ut;
  uname(&ut);
  printf("user is: %d\n", getuid());
  printf("epoch is: %d\n", time(time_t));
  printf("utsname is: %s %s %s %s %s %s\n", ut.sysname, ut.nodename, ut.release, ut.version, ut.machine, ut.domainname);

  for(int i = 0; i < 256; i++){
  	  putchar(i);
  }

  struct stat st;
  int fd = open("/test", O_RDWR);
  fstat(fd, &st);
  printf("/test time: %d\n", st.lmtime);

  int rc = setuid(1);
  printf("setuid: %s\n", strerror(rc));
  exec("/bin/sh", argv);
  return 0;
}
