#include "../include/types.h"
#include "../include/stat.h"
#include "../include/fcntl.h"
#include "../include/user.h"
#include "../include/x86.h"
#include "../include/tty.h"

char *
strdup (const char *s) {
  size_t len = strlen (s) + 1;
  void *new = malloc (
len);
  if (new == NULL)
    return NULL;
  return (char *) memmove (new, s, len);
}

int
abs (int i)
{
  return i < 0 ? -i : i;
}

char *
strcat(char *s1, const char *s2)
{	/* copy char s2[] to end of s1[] */
	char *s;
	for (s = s1; *s != '\0'; ++s)
		;			/* find end of s1[] */
	for (; (*s = *s2) != '\0'; ++s, ++s2)
		;			/* copy s2[] to end */
	return (s1);
}

int
toupper(int c)
{
  return islower (c) ? c - 'a' + 'A' : c;
}

int
islower(int c)
{
    return c >= 'a' && c <= 'z' ? 0x02 : 0;
}

char *
strrchr (register const char *s, int c)
{
  char *rtnval = 0;

  do {
    if (*s == c)
      rtnval = (char*) s;
  } while (*s++);
  return (rtnval);
}

static int is_leap_year(int year) {
    if (year % 4)   return 0;
    if (year % 100) return 1;
    if (year % 400) return 0;
    return 1;
}

struct tm *
localtime(const unsigned long *timer) {
    static struct tm tm;
    unsigned long t = *timer;
    int year, month;
    const int days_per_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    tm.tm_sec = t % 60;
    t /= 60;
    tm.tm_min = t % 60;
    t /= 60;
    tm.tm_hour = t % 24;
    t /= 24;

    // Calculate day of week (1970-01-01 was Thursday)
    tm.tm_wday = (t + 4) % 7;

    year = 1970;
    while (1) {
        int days_in_year = is_leap_year(year) ? 366 : 365;
        if (t < days_in_year) break;
        t -= days_in_year;
        year++;
    }
    tm.tm_year = year - 1900;

    int leap = is_leap_year(year);
    for (month = 0; month < 12; month++) {
        int days = days_per_month[month];
        if (month == 1 && leap) days++; // February leap day
        
        if (t < days) break;
        t -= days;
    }
    tm.tm_mon = month;
    tm.tm_mday = t + 1;

    return &tm;
}

int
isdigit(int c)
{
	return (unsigned)c - '0' < 10;
}

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

int
getchar()
{
  char c;
  if (read(0, &c, 1) <= 0)
    return -1;
  return (unsigned char)c;
}

int
getc(int fd)
{
  char c;
  if (read(fd, &c, 1) <= 0) // read returns 0 on EOF or -1 on error
    return -1;               // Hence, -1 indicates EOF/error
  return (unsigned char)c;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

int
fclose(int fd)
{
  return close(fd);
}
