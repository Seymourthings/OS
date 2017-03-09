#include <spinlock.h>
#include <limits.h>
#include <file_syscall.h>

/* Global Semaphore for sys_exit & menu */
extern struct semaphore * g_sem;

int sys_getpid(int32_t *retval);
void sys_exit(int exitcode);
