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

/*
 * Use these stubs to test your reader-writer locks.
 */
//static struct lock *testlock = NULL;
//static struct rwlock *testrw = NULL;

//struct spinlock status_lock;
//static bool test_status = TEST161_FAIL;

int rwtest(int nargs, char **args) {
	(void)nargs;
	(void)args;
/*		
	int i, result;

	kprintf_n("Starting rwt1...\n");
	for (i=0; i<CREATELOOPS; i++) {
		kprintf_t(".");
		testlock = lock_create("testlock");
		if (testlock == NULL) {
			panic("lt1: lock_create failed\n");
		}
		donesem = sem_create("donesem", 0);
		if (donesem == NULL) {
			panic("lt1: sem_create failed\n");
		}
		if (i != CREATELOOPS - 1) {
			lock_destroy(testlock);
			sem_destroy(donesem);
		}
	}
	spinlock_init(&status_lock);
	test_status = TEST161_SUCCESS;

	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		result = thread_fork("synchtest", NULL, locktestthread, NULL, i);
		if (result) {
			panic("lt1: thread_fork failed: %s\n", strerror(result));
		}
	}
	for (i=0; i<NTHREADS; i++) {
		kprintf_t(".");
		P(donesem);
	}

	lock_destroy(testlock);
	sem_destroy(donesem);
	testlock = NULL;
	donesem = NULL;

	kprintf_t("\n");
	success(test_status, SECRET, "lt1");
*/

	kprintf_n("rwt1 unimplemented\n");
	success(TEST161_FAIL, SECRET, "rwt1");

	return 0;
}

int rwtest2(int nargs, char **args) {
	(void)nargs;
	(void)args;
/*	
	kprintf_n("Starting rwt2...\n");
        kprintf_n("(This test panics on success!)\n");
	
	testrw = rwlock_create("testrw");
	if(testrw == NULL){
		panic("rwt2:rwlock_create failed\n");
	}
	secprintf(SECRET, "Should panic...", "lt2");

	rwlock_acquire_read(testrw);
	rwlock_destroy(testrw);
	
	 Should not get here on success. 
	success(test_status, SECRET, "rwt2");
	
	testrw = NULL;	
*/
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
	test_status = TEST161_SUCCESS;
	success(test_status,SECRET, "rwt3");
*/
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
