/*
 * All the contents of this file are overwritten during automated
 * testing. Please consider this before changing anything in this file.
 */

#include <types.h>
#include <lib.h>
#include <clock.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
#include <kern/test161.h>
#include <spinlock.h>

#define CREATELOOPS 8
#define NSEMLOOPS     63
#define NLOCKLOOPS    120
#define NCVLOOPS      5
#define NTHREADS      32
#define SYNCHTEST_YIELDER_MAX 16

/*static volatile unsigned long testval1;
static volatile unsigned long testval2;
static volatile unsigned long testval3;*/

/*
 * Use these stubs to test your reader-writer locks.
 */

//static struct rwlock *testrw = NULL;
//static struct semaphore *donesem = NULL;


struct spinlock status_lock;
static bool test_status = TEST161_FAIL;

/*static
bool
failif(bool condition) {
        if (condition) {
                spinlock_acquire(&status_lock);
                test_status = TEST161_FAIL;
                spinlock_release(&status_lock);
        }
        return condition;
}
*/

/*static
void
rwlocktestthread(void *junk, unsigned long num)
{
	(void)junk;

	int i;

	for (i=0; i<NLOCKLOOPS; i++) {
		kprintf_t(".");
		rwlock_acquire_read(testrw);
		random_yielder(4);

		testval1 = num;
		testval2 = num*num;
		testval3 = num%3;

		if (testval2 != testval1*testval1) {
			goto fail;
		}
		random_yielder(4);

		if (testval2%3 != (testval3*testval3)%3) {
			goto fail;
		}
		random_yielder(4);

		if (testval3 != testval1%3) {
			goto fail;
		}
		random_yielder(4);

		if (testval1 != num) {
			goto fail;
		}
		random_yielder(4);

		if (testval2 != num*num) {
			goto fail;
		}
		random_yielder(4);

		if (testval3 != num%3) {
			goto fail;
		}
		random_yielder(4);
		
		if (testrw->rw_sem->sem_count == 0) {
			goto fail;
		}

		random_yielder(4);

		rwlock_release_read(testrw);
	}

	Check for solutions that don't track ownership properly

	if (testrw->rw_sem->sem_count != 0) {
		goto fail2;
	}

	V(donesem);
	return;

fail:
	rwlock_release_read(testrw);
fail2:
	failif(true);
	V(donesem);
	return;
}


static
void
rwlocktestthread2(void *junk, unsigned long num)
{
	(void)junk;

	int i;

	for (i=0; i<NLOCKLOOPS; i++) {
		kprintf_t(".");
		rwlock_acquire_write(testrw);
		rwlock_acquire_read(testrw);
		random_yielder(4);

		testval1 = num;
		testval2 = num*num;
		testval3 = num%3;

		if (testval2 != testval1*testval1) {
			goto fail;
		}
		random_yielder(4);

		if (testval2%3 != (testval3*testval3)%3) {
			goto fail;
		}
		random_yielder(4);

		if (testval3 != testval1%3) {
			goto fail;
		}
		random_yielder(4);

		if (testval1 != num) {
			goto fail;
		}
		random_yielder(4);

		if (testval2 != num*num) {
			goto fail;
		}
		random_yielder(4);

		if (testval3 != num%3) {
			goto fail;
		}
		random_yielder(4);
		
		if (testrw->rw_thread == NULL) {
			goto fail;
		}

		random_yielder(4);

		rwlock_release_write(testrw);
	}

	 Check for solutions that don't track ownership properly 

	if (testrw->rw_thread != NULL) {
		goto fail2;
	}

	V(donesem);
	return;

fail:
	rwlock_release_write(testrw);
fail2:
	failif(true);
	V(donesem);
	return;
}

*/

int rwtest(int nargs, char **args) {
        (void)nargs;
        (void)args;

    /*    int i, result;

        kprintf_n("Starting rwt1...\n");
        for (i=0; i<CREATELOOPS; i++) {
                kprintf_t(".");
                testrw = rwlock_create("testrw");
                if (testrw == NULL) {
                        panic("rwt1: rwlock_create failed\n");
                }
                donesem = sem_create("donesem", 0);
                if (donesem == NULL) {
                        panic("rwt1: sem_create failed\n");
                }
                if (i != CREATELOOPS - 1) {
                        rwlock_destroy(testrw);
                        sem_destroy(donesem);
                }
        }
        spinlock_init(&status_lock);
        test_status = TEST161_SUCCESS;

        for (i=0; i<NTHREADS; i++) {
                kprintf_t(".");
                result = thread_fork("synchtest", NULL, rwlocktestthread, NULL, i);
                if (result) {
                        panic("rwt1: thread_fork failed: %s\n", strerror(result));
                }
        }
*/
        success(test_status, SECRET, "rwt1");

        return 0;
}


int rwtest2(int nargs, char **args) {
        (void)nargs;
        (void)args;
/*
        int i, result;

        kprintf_n("Starting rwt2...\n");
        for (i=0; i<CREATELOOPS; i++) {
                kprintf_t(".");
                testrw = rwlock_create("testrw");
                if (testrw == NULL) {
                        panic("rwt2: rwlock_create failed\n");
                }
                donesem = sem_create("donesem", 0);
                if (donesem == NULL) {
                        panic("rwt2: sem_create failed\n");
                }
                if (i != CREATELOOPS - 1) {
                        rwlock_destroy(testrw);
                        sem_destroy(donesem);
                }
        }
        spinlock_init(&status_lock);
        test_status = TEST161_SUCCESS;

        for (i=0; i<NTHREADS; i++) {
                kprintf_t(".");
                result = thread_fork("synchtest", NULL, rwlocktestthread2, NULL, i);
                if (result) {
                        panic("rwt2: thread_fork failed: %s\n", strerror(result));
                }
        }
*/
        success(test_status, SECRET, "rwt2");

        return 0;
}
	
int rwtest3(int nargs, char **args) {
	(void)nargs;
	(void)args;
/*
	kprintf_n("Starting rwt3...\n");
	testrw = rwlock_create("testrw");
	if(testrw == NULL){
                panic("rwt3:rwlock_create failed\n");
        }

	rwlock_acquire_write(testrw);
	rwlock_release_write(testrw);
	if(testrw->rw_sem->sem_count == 0){
		test_status = TEST161_SUCCESS;
	}*/
	success(test_status,SECRET, "rwt3");

	return 0;
}

int rwtest4(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt4 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt4");

	return 0;
}

int rwtest5(int nargs, char **args) {
	(void)nargs;
	(void)args;

	kprintf_n("rwt5 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt5");

	return 0;
}
