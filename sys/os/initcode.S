# Initial process execs /init.
# This code runs in user space.

#include <syscall.h>
#include <traps.h>


# exec(init, argv)

.globl start
start:
	# exec("/sbin/init", argv)
	movl	$SYS_exec, %eax
	movl	$init, %ebx		# arg0 = path
	movl	$argv, %ecx		# arg1 = argv
	int	$T_SYSCALL

exit:
	movl	$SYS_exit, %eax
	movl	$0, %ebx
	int	$T_SYSCALL
	jmp	exit

init:
	.string "/sbin/init\0"

.p2align 2
argv:
	.long init
	.long 0

