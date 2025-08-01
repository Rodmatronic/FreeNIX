#include "../include/types.h"
#include "../include/stat.h"
#include "../include/user.h"
#include "../include/stdarg.h"

char putchar_buf[512];

typedef void (*putch_fn)(char, void*);

void
putchar(char c)
{
  write(1, &c, 1);
}

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(putch_fn putch, void *ctx, int xx, int base, int sgn, int width, int zero_pad)
{
    static char digits[] = "0123456789ABCDEF";
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if(sgn && xx < 0) {
        neg = 1;
        x = -xx;
    } else {
        x = xx;
    }

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while((x /= base) != 0);

    int total_digits = i;
    int total_chars = total_digits + (neg ? 1 : 0);
    int pad = width > total_chars ? width - total_chars : 0;

    if (zero_pad) {
        if (neg) putch('-', ctx);
        for (int j = 0; j < pad; j++) putch('0', ctx);
        while (--i >= 0) putch(buf[i], ctx);
    }
    else {
        for (int j = 0; j < pad; j++) putch(' ', ctx);
        if (neg) putch('-', ctx);
        while (--i >= 0) putch(buf[i], ctx);
    }
}

static void
vprintfmt(putch_fn putch, void *ctx, const char *fmt, va_list ap)
{
    char *s;
    int c, i, state;
    int width, zero_pad;

    state = 0;
    for(i = 0; fmt[i]; i++) {
        c = fmt[i] & 0xff;
        if(state == 0) {
            if(c == '%') {
                state = '%';
                width = 0;
                zero_pad = 0;
            } else {
                putch(c, ctx);
            }
        } else if(state == '%') {
            if(c == '0') {
                zero_pad = 1;
                i++;
                c = fmt[i] & 0xff;
            }
            
            while(c >= '0' && c <= '9') {
                width = width * 10 + (c - '0');
                i++;
                c = fmt[i] & 0xff;
            }

            if(c == 'l') {
                i++;
                c = fmt[i] & 0xff;
            }

            if(c == 'd') {
                printint(putch, ctx, va_arg(ap, int), 10, 1, width, zero_pad);
            } else if(c == 'x' || c == 'p') {
                printint(putch, ctx, va_arg(ap, int), 16, 0, width, zero_pad);
            } else if(c == 's') {
                s = va_arg(ap, char*);
                if(s == 0) s = "(null)";
                while(*s != 0) {
                    putch(*s, ctx);
                    s++;
                }
            } else if(c == 'c') {
                putch(va_arg(ap, int), ctx);
            } else if(c == '%') {
                putch(c, ctx);
            } else {
                putch('%', ctx);
                putch(c, ctx);
            }
            state = 0;
            width = 0;
            zero_pad = 0;
        }
    }
}

// Helper for file descriptor output
static void
putch_fd(char ch, void *ctx)
{
    int *fd = (int*)ctx;
    putc(*fd, ch);
}

// Helper for buffer output
static void
putch_buf(char ch, void *ctx)
{
    char **buf = (char**)ctx;
    *(*buf)++ = ch;
}

static void
vprintf(int fd, const char *fmt, va_list ap)
{
    vprintfmt(putch_fd, &fd, fmt, ap);
}

void
vcprintf(const char *fmt, va_list ap)
{
  vprintf(1, fmt, ap);  // FD 1 = console output
}

void
printf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
  va_end(ap);
}

void
fprintf(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
  va_end(ap);
}

int
vsprintf(char *buf, const char *fmt, va_list ap)
{
    char *start = buf;
    vprintfmt(putch_buf, &buf, fmt, ap);
    *buf = '\0';
    return buf - start;
}

int
sprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    int rc;

    va_start(ap, fmt);
    rc = vsprintf(buf, fmt, ap);
    va_end(ap);
    return rc;
}

void
puts(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
  va_end(ap);
}
