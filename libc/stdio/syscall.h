#ifndef COSMOPOLITAN_LIBC_STDIO_SYSCALL_H_
#define COSMOPOLITAN_LIBC_STDIO_SYSCALL_H_
COSMOPOLITAN_C_START_

#define SYS_gettid    1
#define SYS_getrandom 2
#define SYS_getcpu    3

long syscall(long, ...) libcesque;

long cosmo_syscall2(long, long, int) asm("__syscall2");
long cosmo_syscall3(long, long, long, int) asm("__syscall3");
long cosmo_syscall4(long, long, long, long, int) asm("__syscall4");
long cosmo_syscall5(long, long, long, long, long, int) asm("__syscall5");
long cosmo_syscall6(long, long, long, long, long, long, int) asm("__syscall6");

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_SYSCALL_H_ */
