#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"
#include "../include/fcntl.h"

char	stdbuf[BUFSIZ];

main(argc, argv)
char **argv;
{
	int fflg = 0;
	int fi;
	register int c;
//	int ino, dev = -1;
//	int dev = -1;
	struct stat statb;

	for( ; argc>1 && argv[1][0]=='-'; argc--,argv++) {
		switch(argv[1][1]) {
		case 0:
			break;
		case 'u':
			continue;
		}
		break;
	}
	stat(stdout, &statb);
	statb.mode &= S_IFMT;
	if (statb.mode!=S_IFCHR) {
//		dev = statb.dev;
//		ino = statb.ino;
	}
	if (argc < 2) {
		argc = 2;
		fflg++;
	}
	while (--argc > 0) {
		if (fflg || ((*++argv)[0]=='-' && (*argv)[1]=='\0'))
			fi = stdin;
		else {
			fi = open(*argv, O_RDONLY);
			if(fi == -1) {
				fprintf(stderr, "cat: can't open %s\n", *argv);
				continue;
			}
		}
		stat(fi, &statb);
//		if (statb.dev==dev) {
//			fprintf(stderr, "cat: input %s is output\n",
//			   fflg?"-": *argv);
//			fclose(fi);
//			continue;
//		}
		while ((c = getc(fi)) != EOF)
			putchar(c);
		if (fi!=stdin)
			fclose(fi);
	}
	return(0);
}

