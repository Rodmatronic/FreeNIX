/*
 * simple vi - visual text editor
 */

#include <errno.h>
#include <stdio.h>
#include <stat.h>
#include <fcntl.h>
#include <types.h>

int x=0;
int y=0;
int tempfd;
char * filebuf = "\0";
char * sourcefb = "\0";
int status;
int exists;

void viexit(int s);
void echoback();
int copy(char *input, char *dest);

/*
 * sync the ansi cursor with reality
 */

void syncxy() {
	if (x < 1)
		x=1;
	if (x > 80)
		x=80;
	if (y < 1)
		y=1;
	if (y > 24)
		y=24;
	printf("\033[%d;%dH", y, x);
}

void gotoxy(x, y) {
	printf("\033[%d;%dH", y, x);
}

void enable(){
	struct ttyb t;
	gtty(&t);
	t.tflags = ECHO;
	t.tflags |= RAW;
	stty(&t);
}

void disable(){
	struct ttyb t;
	gtty(&t);
	t.tflags |= RAW;
	t.tflags &= ~ECHO;
	stty(&t);
}

void
tobottom() {
	gotoxy(0, 24);
	for(int i=0;i<79;i++)
		write(stdout, " ", 1);
	gotoxy(0, 24);
	printf("\033[24;0H");
}

int cursor_offset(int fd, int line, int col) {
	lseek(fd, 0, SEEK_SET);
	int current = 1;
	int pos_in_line = 1;
	char c;
	int offset = 0;
	while (read(fd, &c, 1) == 1) {
		if (current == line && pos_in_line == col)
			break;
		offset++;
		if (c == '\n') {
			current++;
			pos_in_line = 1;
		} else {
			pos_in_line++;
		}
	}
	return offset;
}

int line_start(int fd, int line) {
	lseek(fd, 0, SEEK_SET);
	int current = 1;
	char c;
	int offset = 0;
	while (read(fd, &c, 1) == 1) {
		if (current == line)
			break;
		offset++;
		if (c == '\n')
			current++;
	}
	return offset;
}

int line_end(int fd, int line) {
	lseek(fd, 0, SEEK_SET);
	int current = 1;
	char c;
	int offset = 0;
	while (read(fd, &c, 1) == 1) {
		if (current == line && c == '\n')
			break;
		offset++;
		if (c == '\n')
			current++;
	}
	return offset;
}

int line_length(int fd, int line) {
	lseek(fd, 0, SEEK_SET);
	int current = 1;
	int len = 0;
	char c;
	while (read(fd, &c, 1) == 1) {
		if (current == line) {
			if (c == '\n' || c == '\r')
				break;
			len++;
		}
		if (c == '\n')
			current++;
	}
	return len;
}

void
insertmode() {
	enable();
	tobottom();
	write(stdout, "I", 1);
	disable();
	syncxy();
	unsigned char c;
	while (read(0, &c, 1) == 1) {
		switch (c) {
		case 0x00: // up
			y--;
			break;
		case 0x01: // down
			y++;
			break;
		case 0x02: // left
			x--;
			break;
		case 0x03: // right
			if (x < line_length(tempfd, y))
				x++;
		case '\033': // esc
			enable();
			tobottom();
			disable();
			return;
		case '\b':
			return;
		default: 
			int off = cursor_offset(tempfd, y, x);
			lseek(tempfd, off, SEEK_SET);
			write(tempfd, &c, 1);
			enable();
			gotoxy(x, y);
			printf("%c", c);
			x++;
			disable();
			break;
		}
		syncxy();
	}
}

void
commandmode() {
	char c;
	char buf[128];
	int len=0;
	struct stat st;
	memset(buf, 0, sizeof(buf));

	enable();
	tobottom();
	write(stdout, ":", 1);
	while (read(0, &c, 1) == 1) {
		if (c == '\n' || c == '\r') {
			buf[len] = '\0';  // terminate
			if (strcmp(buf, "q") == 0) {
				viexit(0);
			} else if (strcmp(buf, "w") == 0) {
				copy(filebuf, sourcefb);
				tobottom();
				stat(sourcefb, &st);
				printf("%s: %d bytes written.", basename(sourcefb), st.st_size);
				disable();
				return;
			} else if (strcmp(buf, "wq") == 0) {
				copy(filebuf, sourcefb);
				viexit(0);
			} else {
				tobottom();
				printf("Unknown command: %s", buf);
				disable();
			}
			return;  // exit command mode
		} else if (c == '\033') {
			tobottom();
			disable();
			return;
		} else if (c == '\b') {  // backspace
			if (len > 0) {
				len--;
				printf("\b \b");
			}
		} else if (len < (int)sizeof(buf) - 1) {
			buf[len++] = c;
		}
	}
}


void
viexit(int s) {
	printf("\033[25;0H");
	struct ttyb t;
	gtty(&t);
	t.tflags = ECHO;
	stty(&t);
	unlink(filebuf);
	close(tempfd);
	exit(s);
}

/*
 * echo temp file contents to display -- used on refresh ONLY!
 */

void
echoback() {
	enable();
	gotoxy(1, 2);
	for (int i=0; i < 22;i++) {
		write(stdout, "~\n", 2);
	}
	gotoxy(1, 1);
	char buf[1024]; // TODO: replace with fsize
	ssize_t bytesread;
	lseek(tempfd, 0, SEEK_SET);
	while ((bytesread = read(tempfd, buf, sizeof(buf))) > 0) {
		if (write(stdout, buf, bytesread) != bytesread) {
			perror("write");
			close(tempfd);
			exit(1);
		}
	}

	if (bytesread < 0)
		perror("read");
	disable();
}

int
copy(char * input, char * dest)
{
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		return(EXIT_FAILURE);
	} else if (pid == 0) {
		execl("/bin/cp", "cp", input, dest, (char *)NULL);
		perror("execl");
		return(EXIT_FAILURE);
	} else {
		wait(NULL);
	}
	return 0;
}

int
main(int argc, char ** argv)
{
	if (argc < 2) {
		fprintf(stderr, "Please provide a file\n");
		exit(1);
	}
	char * buf = "\0";
	struct stat st;
	sourcefb = argv[1];
	stat(sourcefb, &st);
	sprintf(buf, "/tmp/%s", basename(argv[1]));
	filebuf = buf;
	copy(argv[1], buf);
	tempfd = open(buf, O_RDWR);
	printf("\033[2J");
	echoback();
	tobottom();
	printf("%s: %d bytes.", basename(sourcefb), st.st_size);
	disable();
	syncxy();
	unsigned char c;
	while (read(0, &c, 1) == 1) {
		switch (c) {
		case 0x00: // up
			y--;
			break;
		case 0x01: // down
			y++;
			break;
		case 0x02: // left
			x--;
			break;
		case 0x03: // right
			x++;
			break;
		case ':':
			commandmode();
			break;
		case 'i':
			insertmode();
			break;
		default:
			break;
		}
		syncxy();
	}
	exit(1);
}
