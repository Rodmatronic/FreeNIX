#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"

struct utsname	unstr, *un;

main(argc, argv)
char **argv;
int argc;
{
	int	sflg=1, nflg=0, rflg=0, vflg=0, mflg=0, errflg=0;
	int	optlet;

	un = &unstr;
	uname(un);

	while((optlet=getopt(argc, argv, "asnrvm")) != EOF) switch(optlet) {
	case 'a':
		sflg++; nflg++; rflg++; vflg++; mflg++;
		break;
	case 's':
		sflg++;
		break;
	case 'n':
		nflg++;
		break;
	case 'r':
		rflg++;
		break;
	case 'v':
		vflg++;
		break;
	case 'm':
		mflg++;
		break;
	case '?':
		errflg++;
	}
	if(errflg) {
		fprintf(stderr, "usage: uname [-snrvma]\n");
		exit(1);
	}
	if(nflg | rflg | vflg | mflg) sflg--;
	if(sflg)
		fprintf(stdout, "%s", un->sysname);
	if(nflg) {
		if(sflg) putchar(' ');
		fprintf(stdout, "%s", un->nodename);
	}
	if(rflg) {
		if(sflg | nflg) putchar(' ');
		fprintf(stdout, "%s", un->release);
	}
	if(vflg) {
		if(sflg | nflg | rflg) putchar(' ');
		fprintf(stdout, "%s", un->version);
	}
	if(mflg) {
		if(sflg | nflg | rflg | vflg) putchar(' ');
		fprintf(stdout, "%s", un->machine);
	}
	putchar('\n');
	exit(0);
}
