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

int sys_read(int fd, void *buf, size_t buflen, int32_t *retval);
int sys_write(int fd, void *buf, size_t buflen, int32_t *retval);
int sys_open(const char *filename, int flags, int mode, int32_t *retval);
int sys_close(int fd, int32_t *retval);
off_t sys_lseek(int fd, off_t pos, const_userptr_t whence, off_t *offset);
int sys_dup2(int fd,int newfd, int32_t *retval);
int sys__getcwd(void *buf, size_t buflen, int32_t *retval);
int sys_chdir(const char *pathname, int32_t *retval);
#endif /*_FILE_SYSCALL_H_ */
