#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"
#include "../include/fcntl.h"

main(argc,argv)
int argc;
char *argv[];
{
int i;
static int force = 1;

for(i = 1 ; i < argc ; ++i)
	if( strcmp(argv[i], "-c") )
		touch(force, argv[i]);
	else
		force = 1;
exit(0);
}

touch(force, name)
int force;
char *name;
{
struct stat stbuff;
char junk[1];
int fd;

if(stat(name,&stbuff) < 0)
	if(force)
		goto create;
	else
		{
		fprintf(stderr, "touch: file %s does not exist.\n", name);
		return 1;
		}

if(stbuff.size == 0)
	goto create;

if( (fd = open(name, 2)) < 0)
	goto bad;

if( read(fd, junk, 1) < 1)
	{
	close(fd);
	goto bad;
	}
//lseek(fd, 0L, 0);
if( write(fd, junk, 1) < 1 )
	{
	close(fd);
	goto bad;
	}
close(fd);
return 0;

bad:
	fprintf(stderr, "Cannot touch %s\n", name);
	return 1;

create:
	if( (fd = open(name, O_CREATE | O_RDONLY)) < 0)
		goto bad;
	close(fd);
	return 0;
}

