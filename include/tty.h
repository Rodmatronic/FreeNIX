// ../include/tty.h
#ifndef _TTY_H_
#define _TTY_H_

#define ECHO	010
#define RAW	020

#include "../include/spinlock.h"

struct ttyb {
	int	speeds;
	char	erase, kill;
	int	tflags;
};

struct cons {
  struct spinlock lock;
  int locking;
};

#endif
