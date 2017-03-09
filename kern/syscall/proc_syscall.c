#include <types.h>
#include <spl.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vnode.h>
#include <kern/unistd.h>
#include <proc_syscall.h>
#include <kern/wait.h>

int pid_stack[PID_MAX/2];
int stack_index;
int g_pid;

/* Returns the current process's ID */
int sys_getpid(int32_t *retval){
	*retval = curproc->pid;
	return 0;
}

/* Curproc exits */
void sys_exit(int exitcode){
	int ppid = curproc->pid;
	
	/*
	 * Since this is exiting, if any children were
	 * forked from here, we must assign them a new parent.
	 * Assign the kproc (kernel) as the parent of each
	 */

	int index = 0;
	while(index < PROC_MAX){
		if(proc_table[index] != NULL){
			if(proc_table[index]->ppid == ppid){
				lock_acquire(proc_table[index]->lock);
				proc_table[index]->ppid = kproc->pid;
				lock_release(proc_table[index]->lock);
			}
		
		}
		
		index++;
	}
	
	curproc->exitcode = _MKWAIT_EXIT(exitcode);
	curproc->exited = true;

	/* Increment sem count - main/menu.c */	
	V(g_sem);
	thread_exit();
}
