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
/* Write to a user file */

int sys_read(int fd, void *buf, size_t buflen, int32_t *retval){
	
        if (fd < 0 || fd >= OPEN_MAX){
                *retval = -1;
                return EBADF;
        }

        if(curproc->file_table[fd] == NULL){
                *retval = -1;
                return EBADF;
        }
	
	/* Not using a semaphore here because processes<->threads are 1 to 1 */
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


        uio_uinit(&iovec, &uio, buf, buflen, curproc->file_table[fd]->offset, UIO_READ);
        err = VOP_READ(curproc->file_table[fd]->vnode, &uio);


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

