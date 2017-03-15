#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <spl.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vnode.h>
#include <kern/unistd.h>
#include <proc_syscall.h>
#include <kern/wait.h>
#include <lib.h>
#include <copyinout.h>
#include <vfs.h>
#include <kern/syscall.h>
#include <syscall.h>
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
	/*
	 * Since this is exiting, if any children were
	 * forked from here, we must assign them a new parent.
	 * Assign the kproc (kernel) as the parent of each
	 */
		 
	lock_acquire(curproc->lock);
	int index = 0;
	while(index < PROC_MAX){
		if(proc_table[index] != NULL){
			if(proc_table[index]->ppid == curproc->pid){
				proc_table[index]->ppid = 0;
			}
		}
		index++;
	}	
	curproc->exitcode = _MKWAIT_EXIT(exitcode);
	curproc->exited = true;
	cv_broadcast(curproc->cv, curproc->lock);
	
	lock_release(curproc->lock);
	
	/* Increment sem count - main/menu.c */	
	V(g_sem);
	thread_exit();
}


/* See thread_fork in thread.c 
 * At this point child is curproc
 */
static void child_entrypoint(void* data1, unsigned long data2) {
	struct trapframe tf;
	tf = *((struct trapframe *) data1);
	(void)data2;	
	/*Should be a new process, stolen from runprogram.c */

	tf.tf_a3 = 0;
	tf.tf_v0 = 0;
	tf.tf_epc += 4;

	/* Set curproc address to that of passed in address (the parent's) */

	/* No args yet */
	mips_usermode(&tf);
}

pid_t sys_fork(struct trapframe *tf_parent, int32_t *retval){
	struct proc *proc_child;
	struct trapframe *tf_temp;
	int err;

	/*---Create proccess; assign ppid--- */
	proc_child = proc_create("Proc");
	if(proc_child == NULL){
		*retval = -1;
		return ENOMEM;
	}
	lock_acquire(proc_child->lock);
	proc_child->ppid = curproc->pid;
	lock_release(proc_child->lock);
	/* Allocating space for address and copying into temp var */
	
	err = as_copy(curproc->p_addrspace, &proc_child->p_addrspace);
	if(err){
		*retval = -1;
		proc_destroy(proc_child);
		return ENOMEM;
	}
	
	/*---Allocating space for trapframe to be passed into child_forkentry---*/
	tf_temp = kmalloc(sizeof(*tf_temp));
	if(tf_temp == NULL){
		*retval = -1;
		kfree(tf_temp);
		proc_destroy(proc_child);
		return ENOMEM;
	}
	*tf_temp = *tf_parent;
	
	int index = 0;
	while(index < OPEN_MAX){
		if(curproc->file_table[index] != NULL){
			lock_acquire(curproc->file_table[index]->lock);
			curproc->file_table[index]->count++;
			lock_release(curproc->file_table[index]->lock);
		}
		proc_child->file_table[index] = curproc->file_table[index];
		index++;
	};
	
	err = thread_fork("child thread", proc_child,
			(void*)child_entrypoint,tf_temp,(unsigned long)NULL);
	
	/* The parent is the curproc here */
	lock_acquire(curproc->lock);
	*retval = proc_child->pid;
	lock_release(curproc->lock);
	return 0;
}

struct proc *get_proc(pid_t pid){
	int i = 0;
	for(; i < PROC_MAX; i++){
		if(pid == proc_table[i]->pid){
			return proc_table[i];
		}
	}
	return NULL;
}

pid_t sys_waitpid(pid_t pid, int *status, int options, int32_t *retval){
	int buffer;
	int err;
	struct proc *proc;
	
/*	 Checks for impossible PID's not PID's that don't exist yet */
	if (pid < 0 || pid > PID_MAX){
		*retval = -1;
		return ESRCH;
	}else{
		proc = get_proc(pid);
	}
	
	if(proc == NULL){
		*retval = -1;
		return ESRCH;
	}
	if(status == NULL){
		*retval = -1;
		return EFAULT;
	}

	if(options != 0){
		*retval = -1;
		return EINVAL;
	}
	
	if(pid == curproc->pid){
		*retval = -1;
		return ECHILD;
	}

	
	if(proc->exited){
		lock_acquire(proc->lock);
		*retval = pid;
		lock_release(proc->lock);
		return 0;
	}	


	lock_acquire(proc->lock);	
	while(!proc->exited){

		cv_wait(proc->cv, proc->lock);

	}
	
	
	buffer = curproc->exitcode;
	err = copyout((const void *)&buffer, (userptr_t)status, sizeof(int));
	if(err){
		*retval = -1;
		return EFAULT;
	}
	lock_release(proc->lock);
	
	
	*retval = pid;
	return 0;
	
}

/*sys_execv*/
int sys_execv(char* progname, char** args, int *retval){
	struct addrspace *as;
	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;
	(void)args;	
	result = vfs_open(progname, O_RDONLY, 0, &v);
	if (result) {
		return result;
	}
	
	as = as_create();
	if (as == NULL) {
		vfs_close(v);
		return ENOMEM;
	}
	
	proc_setas(as);
	as_activate();
	
	result = load_elf(v, &entrypoint);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		vfs_close(v);
		return result;
	}
	
	result = as_define_stack(as, &stackptr);
	if (result) {
		/* p_addrspace will go away when curproc is destroyed */
		return result;
	}
	
	
	/* Done with the file now. */
	vfs_close(v);
	*retval = 0;
	
	/* Warp to user mode. */
	//TO DO change args
	enter_new_process(0 /*argc*/, NULL /*userspace addr of argv*/,
			  NULL /*userspace addr of environment*/,
			  stackptr, entrypoint);
	
	/* enter_new_process does not return. */
	panic("enter_new_process returned\n");
	return EINVAL;
}
