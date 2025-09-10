// init: The initial user-level program

#include "../include/types.h"
#include "../include/stat.h"
#include "../include/stdio.h"
#include "../include/fcntl.h"
#include "../include/errno.h"
#include "../include/signal.h"

#define LVLQ		SIGHUP
#define	LVL0		SIGINT
#define	LVL1		SIGQUIT
#define	LVL2		SIGILL
#define	LVL3		SIGTRAP
#define	LVL4		SIGIOT
#define	LVL5		SIGEMT
#define	LVL6		SIGFPE
#define	SINGLE_USER	SIGBUS
#define	LVLa		SIGSEGV
#define	LVLb		SIGSYS
#define	LVLc		SIGPIPE

char	shell[] =	"/bin/sh";
char	su[]	=	"/usr/bin/su";
char	runc[] =	"/etc/rc";
char	utmp[] =	"/etc/utmp";
char	wtmpf[] =	"/usr/adm/wtmp";
char	getty[] = 	"/etc/getty";

int init_signal = 0;

struct {
	char	name[8];
	char	tty;
	char	fill;
	int	time[2];
	int	wfill;
} wtmp;

void
single()
{
	int pid;
	pid = fork();
	if(pid == 0) {
		fprintf(stderr, "INIT: SINGLE USER MODE\n");
		execl(su, su);
		fprintf(stderr, "INIT: execl of %s failed; errno = %d\n", su, errno);
		exit(1);
	}
	while(wait(0) != pid);

}

void
userinit(int argc, char** argv)
{
	if (argc != 2 || argv[1][1] != '\0') {
		fprintf(stderr,"usage: init [0123456SsQqabc]\n");
		exit(0);
	}

	switch (argv[1][0]) {

	case 'Q':
	case 'q':
		init_signal = LVLQ;
		break;

	case '0':
		init_signal = LVL0;
		break;

	case '1':
		init_signal = LVL1;
		break;

	case '2':
		init_signal = LVL2;
		break;

	case '3':
		init_signal = LVL3;
		break;

	case '4':
		init_signal = LVL4;
		break;

	case '5':
		init_signal = LVL5;
		break;

	case '6':
		init_signal = LVL6;
		break;

	case 'S':
	case 's':
		init_signal = SINGLE_USER;
		single();
		break;

	case 'a':
		init_signal = LVLa;
		break;

	case 'b':
		init_signal = LVLb;
		break;

	case 'c':
		init_signal = LVLc;
		break;

	default:
		fprintf(stderr, "usage: init [0123456SsQqabc]\n");
		exit(1);
	}
	exit(0);
}

int
main(int argc, char** argv)
{
  int pid, wpid;
  int i;

  /* Dispose of random users. */
  if (getuid() != 0) {
    printf("init: %s\n", strerror(EPERM));
    exit(1);
  }

  // Are we invoked from the user?
  if (getpid() != 1)
    userinit(argc, argv);

  if(open("/dev/console", O_RDWR) < 0){
    mknod("/dev/console", 1, 1);
    open("/dev/console", O_RDWR);
  }

  // run shell sequence
  dup(0);
  dup(0);

  i = fork();
  if(i == 0) {
    execl("/bin/sh", shell, runc, 0);
    exit(0);
  }
  while(wait(0) != i);
  close(open(utmp, O_CREATE | O_RDWR));
  if ((i = open(wtmpf, 1)) >= 0) {
    wtmp.tty = '~';
    time(wtmp.time);
    write(i, &wtmp, 16);
    close(i);
  }

  for(;;){
    pid = fork();
    if(pid < 0){
      fprintf(stderr, "init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      execl(getty, 0);
      exit(1);
    }
    while((wpid=wait(0)) >= 0 && wpid != pid){}
  }
}
