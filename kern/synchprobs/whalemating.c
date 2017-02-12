/*
 * Copyright (c) 2001, 2002, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Driver code is in kern/tests/synchprobs.c We will
 * replace that file. This file is yours to modify as you see fit.
 *
 * You should implement your solution to the whalemating problem below.
 */

#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>
#include <wchan.h>

/*
 * Called by the driver during initialization.
 */
struct cv *cv;
struct lock *lock;
struct semaphore *male_sem;
struct semaphore *female_sem;
struct semaphore *match_sem;
const char *name = "whale_mating";

void whalemating_init() {
	cv = cv_create(name);
	KASSERT(cv != NULL);
	
	lock = lock_create(name);
	KASSERT(lock != NULL);	
	
	male_sem = sem_create(name, 0);
	KASSERT(male_sem != NULL);

	female_sem = sem_create(name, 0);
	KASSERT(female_sem != NULL);

	match_sem = sem_create(name, 0);
	KASSERT(match_sem != NULL);
}

/*
 * Called by the driver during teardown.
 */

void
whalemating_cleanup() {
	cv_destroy(cv);

        lock_destroy(lock);
        
	sem_destroy(male_sem);
	sem_destroy(female_sem);
        sem_destroy(match_sem);
}

void
male(uint32_t index)
{
	
	V(male_sem);
	if((female_sem->sem_count > 0) && (match_sem->sem_count > 0)){
		male_start(index);
	        male_end(index);
        	P(male_sem);
	}else {
	 	spinlock_acquire(&male_sem->sem_lock);
        	while((female_sem->sem_count == 0) || (match_sem->sem_count == 0)){
                	wchan_sleep(male_sem->sem_wchan, &male_sem->sem_lock);
		}
                wchan_wakeone(male_sem->sem_wchan, &male_sem->sem_lock);
		male_start(index);
                male_end(index);
                P(male_sem);
                spinlock_release(&male_sem->sem_lock);
		
	}
	
	/*
	 * Implement this function by calling male_start and male_end when
	 * appropriate.
	 */
	return;
}

void
female(uint32_t index)
{
	V(female_sem);
        if((male_sem->sem_count > 0) && (match_sem->sem_count > 0)){
                female_start(index);
                female_end(index);
                P(female_sem);
        }else {
                spinlock_acquire(&female_sem->sem_lock);
                while((male_sem->sem_count == 0) || (match_sem->sem_count == 0)){
                        wchan_sleep(female_sem->sem_wchan, &female_sem->sem_lock);
                }
                wchan_wakeone(female_sem->sem_wchan, &female_sem->sem_lock);
                female_start(index);
                female_end(index);
                P(female_sem);
                spinlock_release(&female_sem->sem_lock);
        }

	/*
	 * Implement this function by calling female_start and female_end when
	 * appropriate.
	 */
	return;
}

void
matchmaker(uint32_t index)
{	
	V(match_sem);
        if((male_sem->sem_count > 0) && (female_sem->sem_count > 0)){
                matchmaker_start(index);
                matchmaker_end(index);
                P(match_sem);
        }else {
                spinlock_acquire(&match_sem->sem_lock);
                while((male_sem->sem_count == 0) || (female_sem->sem_count == 0)){
                        wchan_sleep(match_sem->sem_wchan, &match_sem->sem_lock);
                }
                wchan_wakeone(match_sem->sem_wchan, &match_sem->sem_lock);

                matchmaker_start(index);
                matchmaker_end(index);
                P(match_sem);
                spinlock_release(&match_sem->sem_lock);
        }

	/*
	 * Implement this function by calling matchmaker_start and matchmaker_end
	 * when appropriate.
	 */
	return;
}
