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
pid_t g_pid;

/* Returns the current process's ID */
pid_t sys_getpid(int32_t *retval){
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

pid_t sys_fork(struct trapframe *tf_parent, int32_t *retval){
	struct proc *proc_child;
	struct trapframe *tf_child;
	struct addrspace *addr_child;
	int err;

	/*---Create proccess; assign ppid--- */

	proc_child = proc_create("Proc");
	proc->ppid = curproc->pid;

	/* Allocating space for address and copying into temp var */
	addr_child = kmalloc(sizeof(*addr_child));
	err = as_copy(curproc->p_addrspace, &addr_child);
	if(err){
		*retval = -1;
		kfree(addr_child);
		proc_destroy(proc_child);
		return ENOMEM;
	}
	
	/*---Allocating space for trapframe to be passed into child_forkentry---*/
	tf_child = kmalloc(sizeof(*tf_child));
	if(tf_child == NULL){
		*retval = -1;
		kfree(tf_child);
		proc_destroy(proc_child);
		return ENOMEM;
	}
	
	proc_child->file_table = (struct file_handle *)kmalloc(sizeof(struct file_handle *));
	
	int index = 0;
	while(index < OPEN_MAX){
		proc_child->file_table[index] = curproc->file_table[index];
	};
	
	/* Not enough args yet , not sure which trapframe gets passed here
	 * Ben says we copy the trapframe within sys_fork and then call thread fork
	* Does that mean we pass the child's trapframe?
	*/
	err = thread_fork("child thread", proc_child,
			child_entrypoint);
	
	/* The parent is the curproc here */
	*retval = proc->child->pid;
	return 0;
}

/* See thread_fork in thread.c 
 * At this point child is curproc
 */
void child_entrypoint(void* data1, unsigned long data2) {
	struct trapframe *tf;
	struct addrspace * addr;
	tf = (struct trapframe *) data1;
	addr = (struct addrspace *) data2;
	
	/*Should be a new process, stolen from runprogram.c */
	KASSERT(proc_getas() == NULL);

	tf->tf_a3 = 0;
	tf->tf_v0 = 0;
	tf->tf_epc += 4;

	/* Set curproc address to that of passed in address (the parent's) */
	curproc->p_addrspace = addr;
	as_activate();

	/* No args yet */
	mips_usermode();
}
