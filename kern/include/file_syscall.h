#ifndef _FILE_SYSCALL_H_
#define _FILE_SYSCALL_H_

#include <synch.h>
#include <thread.h>
#include <vnode.h>

/* File Handle & File System Syscalls */

struct file_handle {
        struct vnode *vnode;    /* actual file on disk */

        struct lock *lock;      /* synchronization for file */

        int count;              /* number of processes pointing to file */

        unsigned int flags;

        off_t offset;
};

int sys_open(const_useptr_t pathname, int flags, int32_t *retval);
int sys_read(int fd, void *buf, size_t buflen, int32_t *retval);
int sys_write(int fd, void *buf, size_t buflen, int32_t *retval);

#endif /*_FILE_SYSCALL_H_ */
