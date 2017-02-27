#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <limits.h>
#include <spl.h>
#include <proc.h>
#include <current.h>
#include <addrspace.h>
#include <vnode.h>
#include <uio.h>
#include <synch.h>
#include <file_syscall.h>
#include <kern/unistd.h>
#include <copyinout.h>
#include <vfs.h>
/* Write to a user file */

int sys_write(int fd, void *buf, size_t buflen, int32_t *retval){
		
	if (fd < 0 || fd >= OPEN_MAX){
		*retval = -1;
		return EBADF;
	}
	
	if(curproc->file_table[fd] == NULL){
		*retval = -1;
		return EBADF;
	}

	if(curproc->file_table[fd]->lock == NULL){
		*retval = -1;
		return EBADF;
	}
	
	struct uio uio;
        struct iovec iovec;
	int err;

	lock_acquire(curproc->file_table[fd]->lock);

	if (curproc->file_table[fd]->vnode->vn_refcount == 0){
		*retval = -1;
		lock_release(curproc->file_table[fd]->lock);
		return EBADF;
	}
	
	
	uio_uinit(&iovec, &uio, buf, buflen, curproc->file_table[fd]->offset, UIO_WRITE);
	err = VOP_WRITE(curproc->file_table[fd]->vnode, &uio);
    
	
	if(err){
		*retval = -1;
		lock_release(curproc->file_table[fd]->lock);
		return err;
    	}

	curproc->file_table[fd]->offset = uio.uio_offset;
   	*retval = buflen - uio.uio_resid;
	lock_release(curproc->file_table[fd]->lock);
	return 0;
}

int sys_open(const char *filename, int flags, int mode, int32_t *retval){
	int fd;
	int err;
	char *file_dest = (char*)kmalloc(sizeof(char)*PATH_MAX);
	size_t buflen;
	
	//check if filesystem is full
	if(file_dest == NULL){
		*retval = -1;
		kfree(file_dest);
		return ENOSPC;
	}
	//copy userlevel filename to kernel level 
	copyinstr((const_userptr_t)filename, file_dest, PATH_MAX, &buflen);
	
	//check whats in filetable after stdin, stdout, stderr
	for(fd = 3; fd < OPEN_MAX; fd++){
		if(curproc->file_table[fd] == NULL){
			break;
		}
	}
	curproc->file_table[fd] = (struct file_handle*)kmalloc(sizeof(struct file_handle*));
	//check if filesystem is full
	if(fd == OPEN_MAX || curproc->file_table[fd] == NULL){
		*retval = -1;
		kfree(file_dest);
		return ENFILE;
	}
	//open file
	err = vfs_open(file_dest,flags,mode, &curproc->file_table[fd]->vnode);

	if(err){
		*retval = -1;	
		kfree(file_dest);
		return err;
	}

	
	curproc->file_table[fd]->offset = 0;
	curproc->file_table[fd]->flags = flags;
	curproc->file_table[fd]->lock = lock_create(file_dest);
	curproc->file_table[fd]->count = 1;
	
	*retval = fd;

	kfree(file_dest);
	return 0;
	
	
	

	
}

