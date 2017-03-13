/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

/*
 * true - succeed.
 */

static
void
dowait(int nowait, int pid)
{
        int x;

        if (pid<0) {
                /* fork in question failed; just return */
                return;
        }
        if (pid==0) {
                /* in the fork in question we were the child; exit */
                exit(0);
        }

        if (!nowait) {
                if (waitpid(pid, &x, 0)<0) {
                        printf("S");
                }
                else if (WIFSIGNALED(x)) {
                	printf("O");
		}
                else if (WEXITSTATUS(x) != 0) {
                	printf("S");
		}
        }
}


int
main(void)
{
	int pid = fork();
	int time_waste = 0;

	if (pid == -1){
		printf("fork failed\n");
	}
	else if(pid == 0){
		printf("hello from child process!\n");
	}
	else{
		printf("hello from the parent process!\n");		
		
	}
	
	int pid2 = fork();
	if (pid2 == -1){
		printf("fork2 failed\n");
	}
	else if(pid2 == 0){
		printf("child process 2!\n");
	}
	else{
		printf("parent process 2!\n");		
		
	}

	dowait(0, pid);
	while(time_waste < 500000000){
		time_waste++;
	}
	
	exit(0);	
}
