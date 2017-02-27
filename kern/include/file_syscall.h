#ifndef _FILE_SYSCALL_H_
#define _FILE_SYSCALL_H_

#include <synch.h>
#include <thread.h>
#include <vnode.h>

/* File Handle & File System Syscalls */

struct file_handle {
        struct vnode *vnode;    /* actual file on disk */

        struct lock *lock;      /* synchronization for file */

        struct semaphore *sem;  /* Might use this for sys_read */

        int count;              /* number of processes pointing to file */

        unsigned int flags;

        off_t offset;
};

int sys_write(int fd, void *buf, size_t buflen, int32_t *retval);
int sys_open(const char *filename, int flags, int mode, int32_t *retval);
#endif /*_FILE_SYSCALL_H_ */
