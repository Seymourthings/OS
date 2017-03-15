#include <types.h>
#include <spinlock.h>
#include <limits.h>
#include <file_syscall.h>
#include <mips/trapframe.h>
#include <proc.h>
/* Global Semaphore for sys_exit & menu */
extern struct semaphore * g_sem;

pid_t sys_getpid(int32_t *retval);
void sys_exit(int exitcode);
struct proc *get_proc(pid_t pid);
pid_t sys_fork(struct trapframe *tf_parent, int32_t *retval); //trapframe
pid_t sys_waitpid(pid_t pid, int *status, int options, int32_t *retval);
int sys_execv(char* progname, char** args, int *retval);
