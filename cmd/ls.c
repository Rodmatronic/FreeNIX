#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"
#include "../include/fs.h"
#include "../include/fcntl.h"

#define	NFILES	1024
char	*pwdf, *dirf;
char	stdbuf[BUFSIZ];

const int S_IREAD = 0;
const int S_IWRITE = 0;
const int S_ISUID = 0;
const int S_ISGID = 0;
const int S_ISVTX = 0;
const int S_IEXEC = 0;

        #define major(dev) ((dev) >> 8)
        #define minor(dev) ((dev) & 0xFF)

struct lbuf {
	union {
		char	lname[15];
		char	*namep;
	} ln;
	char	ltype;
	short	lnum;
	short	lflags;
	short	lnl;
	short	luid;
	short	lgid;
	long	lsize;
	long	lmtime;
};

int	aflg, dflg, lflg, sflg, tflg, uflg, iflg, fflg, gflg, cflg;
int	rflg	= 1;
long	year;
int	flags;
int	lastuid	= -1;
char	tbuf[16];
long	tblocks;
int	statreq;
struct	lbuf	*flist[NFILES];
struct	lbuf	**lastp = flist;
struct	lbuf	**firstp = flist;
char	*dotp	= ".";

char	*makename();
struct	lbuf *gstat();
long	nblock();

#define	ISARG	0100000

// Swap helper for qsort
static void swap(char *a, char *b, int size) {
    char tmp;
    for (int i = 0; i < size; i++) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

// Quicksort implementation
void qsort(void *base, int nmemb, int size, int (*compar)(const void *, const void *)) {
    if (nmemb <= 1) return;

    char *p = (char *)base;
    int i = 0;

    // Choose last element as pivot
    char *pivot = p + (nmemb - 1) * size;

    for (int j = 0; j < nmemb - 1; j++) {
        if (compar(p + j * size, pivot) < 0) {
            if (i != j) {
                swap(p + i * size, p + j * size, size);
            }
            i++;
        }
    }

    if (i != nmemb - 1) {
        swap(p + i * size, pivot, size);
    }

    qsort(p, i, size, compar);
    qsort(p + (i + 1) * size, nmemb - i - 1, size, compar);
}

char *pwdfname = NULL;

void rewind_pwdf() {
    if (pwdf >= 0) close(pwdf);
    pwdf = open(pwdfname, O_RDONLY);
    if (pwdf < 0) {
        fprintf(stderr, "ls: cannot rewind %s\n", pwdfname);
        exit(1);
    }
}


main(argc, argv)
char *argv[];
{
	int i;
	register struct lbuf *ep, **ep1;
	register struct lbuf **slastp;
	struct lbuf **epp;
	struct lbuf lb;
	char *t;
	int compar();

	time(&lb.lmtime);
	year = lb.lmtime - 6L*30L*24L*60L*60L; /* 6 months ago */
	if (--argc > 0 && *argv[1] == '-') {
		argv++;
		while (*++*argv) switch (**argv) {

		case 'a':
			aflg++;
			continue;

		case 's':
			sflg++;
			statreq++;
			continue;

		case 'd':
			dflg++;
			continue;

		case 'g':
			gflg++;
			continue;

		case 'l':
			lflg++;
			statreq++;
			continue;

		case 'r':
			rflg = -1;
			continue;

		case 't':
			tflg++;
			statreq++;
			continue;

		case 'u':
			uflg++;
			continue;

		case 'c':
			cflg++;
			continue;

		case 'i':
			iflg++;
			continue;

		case 'f':
			fflg++;
			continue;

		default:
			continue;
		}
		argc--;
	}
	if (fflg) {
		aflg++;
		lflg = 0;
		sflg = 0;
		tflg = 0;
		statreq = 0;
	}
	if(lflg) {
		t = "/etc/passwd";
		if(gflg)
			t = "/etc/group";
		pwdf = open(t, O_RDONLY);
	}
	if (argc==0) {
		argc++;
		argv = &dotp - 1;
	}
	for (i=0; i < argc; i++) {
		if ((ep = gstat(*++argv, 1))==NULL)
			continue;
		ep->ln.namep = *argv;
		ep->lflags |= ISARG;
	}
	qsort(firstp, lastp - firstp, sizeof *lastp, compar);
	slastp = lastp;
	for (epp=firstp; epp<slastp; epp++) {
		ep = *epp;
		if ((ep->ltype=='d' && dflg==0) || fflg) {
			if (argc>1)
				printf("\n%s:\n", ep->ln.namep);
			lastp = slastp;
			readdir(ep->ln.namep);
			if (fflg==0)
				qsort(slastp,lastp - slastp,sizeof *lastp,compar);
			if (lflg || sflg)
				printf("total %d\n", tblocks);
			for (ep1=slastp; ep1<lastp; ep1++)
				pentry(*ep1);
		} else
			pentry(ep);
	}
	exit(0);
}

pentry(ap)
struct lbuf *ap;
{
	struct { char dminor, dmajor;};
	register t;
	register struct lbuf *p;
//	register char *cp;

	p = ap;
	if (p->lnum == -1)
		return 1;
	if (iflg)
		printf("%5u ", p->lnum);
	if (sflg)
	printf("%d ", nblock(p->lsize));
	if (lflg) {
		putchar(p->ltype);
		pmode(p->lflags);
		printf("%2d ", p->lnl);
		t = p->luid;
		if(gflg)
			t = p->lgid;
		if (getname(t, tbuf)==0)
			printf("%-6.6s", tbuf);
		else
			printf("%d", t);
		if (p->ltype=='b' || p->ltype=='c')
			printf("%3d,%3d", major((int)p->lsize), minor((int)p->lsize));
		else
			printf("%7ld", p->lsize);

//		struct stat st;
		struct tm tm;
		struct tm cm;

//		fstat(fd, &st);
		epoch_to_tm(p->lmtime, &tm);

		uint current_time = time(0);
		epoch_to_tm(current_time, &cm);

//		unsigned long epoch;
//		epoch_to_tm(time(&epoch), &cm);

		if(tm.tm_year + 1900 < cm.tm_year + 1900)
			printf(" %s %02d  %d ", month[tm.tm_mon], tm.tm_mday, tm.tm_year + 1900); else
			printf(" %s %02d %02d:%02d ", month[tm.tm_mon], tm.tm_mday, tm.tm_hour, tm.tm_min);

//	printf(" %s %d %02d:%02d ", month[tm.tm_mon], tm.tm_mday, tm.tm_hour, tm.tm_min);

	}
	if (p->lflags&ISARG)
		printf("%s\n", p->ln.namep);
	else
		printf("%s\n", p->ln.lname);
	return 0;
}

getname(uid, buf)
int uid;
char buf[];
{
	int j, c, n, i;

	if (uid==lastuid)
		return(0);
	if(pwdf == NULL)
		return(-1);
	rewind_pwdf();
	lastuid = -1;
	do {
		i = 0;
		j = 0;
		n = 0;
		while((c=getc(pwdf)) != '\n') {
			if (c==EOF)
				return(-1);
			if (c==':') {
				j++;
				c = '0';
			}
			if (j==0)
				buf[i++] = c;
			if (j==2)
				n = n*10 + c - '0';
		}
	} while (n != uid);
	buf[i++] = '\0';
	lastuid = uid;
	return(0);
}

long
nblock(size)
long size;
{
	return((size+511)>>9);
}

int	m1[] = { 1, S_IREAD>>0, 'r', '-' };
int	m2[] = { 1, S_IWRITE>>0, 'w', '-' };
int	m3[] = { 2, S_ISUID, 's', S_IEXEC>>0, 'x', '-' };
int	m4[] = { 1, S_IREAD>>3, 'r', '-' };
int	m5[] = { 1, S_IWRITE>>3, 'w', '-' };
int	m6[] = { 2, S_ISGID, 's', S_IEXEC>>3, 'x', '-' };
int	m7[] = { 1, S_IREAD>>6, 'r', '-' };
int	m8[] = { 1, S_IWRITE>>6, 'w', '-' };
int	m9[] = { 2, S_ISVTX, 't', S_IEXEC>>6, 'x', '-' };

int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};

pmode(aflag)
{
	register int **mp;

	flags = aflag;
	for (mp = &m[0]; mp < &m[sizeof(m)/sizeof(m[0])];)
		select(*mp++);
	return 0;
}

select(pairp)
register int *pairp;
{
	register int n;

	n = *pairp++;
	while (--n>=0 && (flags&*pairp++)==0)
		pairp++;
	putchar(*pairp);
	return 0;
}

char *
makename(dir, file)
char *dir, *file;
{
	static char dfile[100];
	register char *dp, *fp;
	register int i;

	dp = dfile;
	fp = dir;
	while (*fp)
		*dp++ = *fp++;
	*dp++ = '/';
	fp = file;
	for (i=0; i<DIRSIZ; i++)
		*dp++ = *fp++;
	*dp = 0;
	return(dfile);
}

readdir(dir)
char *dir;
{
    static struct dirent dentry;
    register int j;
    register struct lbuf *ep;
    int dirf;  // Local file descriptor to avoid conflicts

    if ((dirf = open(dir, O_RDONLY)) == -1) {
        printf("%s unreadable\n", dir);
        return 1;
    }
    tblocks = 0;
    for(;;) {
        // Corrected read call: fd first, check for 16-byte entry
        if (read(dirf, (char *)&dentry, sizeof(dentry)) != sizeof(dentry))
            break;
        // Skip unallocated entries and "."/".." unless -a flag is set
        if (dentry.inum == 0
         || (aflg == 0 && dentry.name[0]=='.' &&
             (dentry.name[1]=='\0' ||
              (dentry.name[1]=='.' && dentry.name[2]=='\0'))))
            continue;
        ep = gstat(makename(dir, dentry.name), 0);
        if (ep==NULL)
            continue;
        // Copy the directory entry name
        for (j=0; j<DIRSIZ; j++)
            ep->ln.lname[j] = dentry.name[j];
    }
    fclose(dirf);
    return 0;
}

struct lbuf *
gstat(file, argfl)
char *file;
{
	struct stat statb;
	register struct lbuf *rep;
	static int nomocore;

	if (nomocore)
		return(NULL);
	rep = (struct lbuf *)malloc(sizeof(struct lbuf));
	if (rep==NULL) {
		fprintf(stderr, "ls: out of memory\n");
		nomocore = 1;
		return(NULL);
	}
	if (lastp >= &flist[NFILES]) {
		static int msg;
		lastp--;
		if (msg==0) {
			fprintf(stderr, "ls: too many files\n");
			msg++;
		}
	}
	*lastp++ = rep;
	rep->lflags = 0;
	rep->lnum = 0;
	rep->ltype = '-';
	if (argfl || statreq) {
		if (stat(file, &statb)<0) {
			printf("%s not found\n", file);
			statb.ino = -1;
			statb.size = 0;
			statb.mode = 0;
			if (argfl) {
				lastp--;
				return(0);
			}
		}
		rep->lnum = statb.ino;
		rep->lsize = statb.size;
		switch(statb.mode&S_IFMT) {

		case S_IFDIR:
			rep->ltype = 'd';
			break;

//		case S_IFBLK:
//			rep->ltype = 'b';
//			rep->lsize = statb.dev;
//			break;

		case S_IFCHR:
			rep->ltype = 'c';
			rep->lsize = statb.dev;
			break;
		}
		rep->lflags = statb.mode & ~S_IFMT;
//		rep->luid = statb.uid;
//		rep->lgid = statb.gid;
		rep->lnl = statb.nlink;
/*		if(uflg)
			rep->lmtime = statb.st_atime;
		else if (cflg)
			rep->lmtime = statb.st_ctime;
		else
			rep->lmtime = statb.st_mtime;*/
		tblocks += nblock(statb.size);
		rep->lmtime = statb.lmtime;
//		tblocks += nblock(statb.size);
	}
	return(rep);
}

compar(pp1, pp2)
struct lbuf **pp1, **pp2;
{
	register struct lbuf *p1, *p2;

	p1 = *pp1;
	p2 = *pp2;
	if (dflg==0) {
		if (p1->lflags&ISARG && p1->ltype=='d') {
			if (!(p2->lflags&ISARG && p2->ltype=='d'))
				return(1);
		} else {
			if (p2->lflags&ISARG && p2->ltype=='d')
				return(-1);
		}
	}
	if (tflg) {
		if(p2->lmtime == p1->lmtime)
			return(0);
		if(p2->lmtime > p1->lmtime)
			return(rflg);
		return(-rflg);
	}
	return(rflg * strcmp(p1->lflags&ISARG? p1->ln.namep: p1->ln.lname,
				p2->lflags&ISARG? p2->ln.namep: p2->ln.lname));
}

