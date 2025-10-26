/*	$OpenBSD: syslog.c,v 1.33 2015/10/31 02:57:16 deraadt Exp $ */
/*
 * Copyright (c) 1983, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "../include/stdio.h"
#include "../include/types.h"
#include "../include/syslog.h"
#include "../include/fcntl.h"
	
/*
 * syslog logs fmt to /var/log/system.log.
 * Prints the date, hostname, program name, and log contents
 */

void
syslog(int pri, const char *fmt, ...)
{
    char datebuf[64];
    char logbuf[512];

    unsigned long epoch = 0;
    epoch = time(epoch);
    struct tm tm;
    epoch_to_tm(epoch, &tm);
    strcpy(datebuf, ctime(&tm));

    va_list ap;
    va_start(ap, fmt);
    vsprintf(logbuf, fmt, ap);
    va_end(ap);

    char finalbuf[576];
    char hostname[128];
    gethostname(hostname, sizeof(hostname));
    sprintf(finalbuf, "%s %s %s: %s", datebuf, hostname, getprogname(), logbuf);

    int fd = open("/var/log/system.log", O_CREAT | O_WRONLY | O_APPEND);
    if (fd >= 0) {
        write(fd, finalbuf, strlen(finalbuf));
        write(fd, "\n", 1);
        close(fd);
    }

    printf("%s\n", finalbuf);
}
