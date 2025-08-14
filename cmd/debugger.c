#include "../include/errno.h"
#include "../include/user.h"
#include "../include/stat.h"
#include "../include/fcntl.h"

char *argv[] = { "sh", 0 };

int
main()
{
  unsigned long time_t = 0;

  struct stat st;
  int fd = open("/test", O_RDWR);
  fstat(fd, &st);
  printf("/test time: %d\n", st.lmtime);

  open("dd/ff/ff", O_CREATE|O_RDWR);

  int rc = setuid(1);
  printf("setuid: %s\n", strerror(rc));
  exec("/bin/sh", argv);
  return 0;
}
