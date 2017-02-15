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
 * Driver code is in kern/tests/synchprobs.c We will replace that file. This
 * file is yours to modify as you see fit.
 *
 * You should implement your solution to the stoplight problem below. The
 * quadrant and direction mappings for reference: (although the problem is, of
 * course, stable under rotation)
 *
 *   |0 |
 * -     --
 *    01  1
 * 3  32
 * --    --
 *   | 2|
 *
 * As way to think about it, assuming cars drive on the right: a car entering
 * the intersection from direction X will enter intersection quadrant X first.
 * The semantics of the problem are that once a car enters any quadrant it has
 * to be somewhere in the intersection until it call leaveIntersection(),
 * which it should call while in the final quadrant.
 *
 * As an example, let's say a car approaches the intersection and needs to
 * pass through quadrants 0, 3 and 2. Once you call inQuadrant(0), the car is
 * considered in quadrant 0 until you call inQuadrant(3). After you call
 * inQuadrant(2), the car is considered in quadrant 2 until you call
 * leaveIntersection().
 *
 * You will probably want to write some helper functions to assist with the
 * mappings. Modular arithmetic can help, e.g. a car passing straight through
 * the intersection entering from direction X will leave to direction (X + 2)
 * % 4 and pass through quadrants X and (X + 3) % 4.  Boo-yah.
 *
 * Your solutions below should call the inQuadrant() and leaveIntersection()
 * functions in synchprobs.c to record their progress.
 */

#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>
#include <current.h>
struct semaphore *q0_sem;
struct semaphore *q1_sem;
struct semaphore *q2_sem;
struct semaphore *q3_sem;
struct semaphore *mux;


//find which semaphore to use  depending on direction
struct semaphore *getSem(uint32_t direction){	
	switch(direction){
		case 0:
			return q0_sem;
			break;
		case 1:
			return q1_sem;
			break;
		case 2:
			return q2_sem;
			break;
		default:
			return q3_sem;
			break;
	}
}

/*
 * Called by the driver during initialization.
 */
void
stoplight_init() {
	mux = sem_create("mutex", 1);
	q0_sem = sem_create("quad 0", 1);
	q1_sem = sem_create("quad 1", 1);
	q2_sem = sem_create("quad 2", 1);
	q3_sem = sem_create("quad 3", 1);
	
	return;
}

/*
 * Called by the driver during teardown.
 */

void stoplight_cleanup() {
	sem_destroy(q0_sem);
	sem_destroy(q1_sem);
	sem_destroy(q2_sem);
	sem_destroy(q3_sem);
	sem_destroy(mux);
	return;
}

void
turnright(uint32_t direction, uint32_t index)
{
	P(mux);
	//try to move into quad
	P(getSem(direction));
	

	V(mux);
	//made it into quad safe tp travel
	inQuadrant(direction,index);
	leaveIntersection(index);
	V(getSem(direction));
	return;
}
void
gostraight(uint32_t direction, uint32_t index)
{
	int flag,quad;
	flag = 1;
	
	quad = ((direction + 3) % 4);
	
	while(flag){

		P(mux);
		//try to move into quad
		P(getSem(direction));
		//check if its safe to travel straight
		if(getSem(quad)->sem_count > 0){
			P(getSem(quad));
			
			V(mux);
			flag = 0;
		}
		//not safe to travel 
		else{
			V(getSem(direction));
			V(mux);
		}
	
		
	}
	//made it into quads safe to travel 
	inQuadrant(direction,index);
	inQuadrant(quad,index);

	leaveIntersection(index);

	V(getSem(direction));
	V(getSem(quad));



	return;
}
void
turnleft(uint32_t direction, uint32_t index)
{	
	int flag,quad,nextQuad;
	flag = 1;
	quad = ((direction + 3) % 4);
	nextQuad = ((direction + 2) % 4);
	
	while(flag){

		P(mux);
		//try to move into quad
		P(getSem(direction));
		
		//check if its safe to travel straight
		if((getSem(quad)->sem_count > 0)){ 
	
			P(getSem(quad));
			//check if its safe to travel left
			if((getSem(nextQuad)->sem_count > 0)){
				
				P(getSem(nextQuad));
				
				V(mux);
				flag = 0;
			}
			else{
				//not safe to travel left
				V(getSem(quad));
				V(getSem(direction));
			}
		}
		else{
			//not safe to travel staright
			V(getSem(direction));
		}
		V(mux);	
	}
	//made it into quads safe to travel
	inQuadrant(direction,index);
	inQuadrant(quad,index);
	inQuadrant(nextQuad,index);


	leaveIntersection(index);
	V(getSem(quad));
	V(getSem(nextQuad));
	V(getSem(direction));
	return;


}
