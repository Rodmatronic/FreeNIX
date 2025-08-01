#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"

int
main(int argc, char **argv)
{
	int signo, pid, res;
	int errlev;
	int errno = 0;

	if (argc <= 1) {
	usage:
		printf("usage: kill [ -signo ] pid ...\n");
		exit(2);
	}
	if (*argv[1] == '-') {
		signo = atoi(argv[1]+1);
		argc--;
		argv++;
	} else
		signo = 15;
	argv++;
	while (argc > 1) {
		if (**argv<'0' || **argv>'9')
			goto usage;
		res = kill(pid = atoi(*argv), signo);
		if (res<0) {
			printf("%u: %s\n", pid, strerror(errno));
			errlev = 1;
		}
		argc--;
		argv++;
	}
	return(errlev);
}

