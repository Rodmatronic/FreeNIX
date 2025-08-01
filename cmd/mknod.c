#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"
#include "../include/fs.h"
#include "../include/fcntl.h"

main(argc, argv)
int argc;
char **argv;
{
	int a, b;

	if(argc != 5) {
		goto usage;
	}
	a = number(argv[3]);
	if(a < 0)
		goto usage;
	b = number(argv[4]);
	if(b < 0)
		goto usage;
	if(mknod(argv[1], a, b) < 0){
		fprintf(stderr, "Could not create device node\n");
		exit(1);
	}
	exit(0);

usage:
	fprintf(stderr, "usage: mknod name b/c major minor\n");
	exit(1);
}

number(s)
char *s;
{
	int n, c;

	n = 0;
	while((c = *s++)) {
		if(c<'0' || c>'9')
			return(-1);
		n = n*10 + c-'0';
	}
	return(n);
}
