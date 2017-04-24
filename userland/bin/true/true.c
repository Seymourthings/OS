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
#include <err.h>

/*
 * true - succeed.
 */
static
	void
dowait(int nowait, int pid)
{
	int x;
	int pid2;

	if (pid<0) {
		/* fork in question failed; just return */
		return;
	}
	if (pid==0) {
		/* in the fork in question we were the child; exit */
		exit(0);
	}

	if (!nowait) {
		pid2 = waitpid(pid, &x, 0);
		printf("PID is: %d \n", pid2);
		if (pid2<0) {
			printf("Failing at waitpid if: With PID: %d \n", pid2);
			errx(1, "waitpid");
		}
		else if (WIFSIGNALED(x)) {
			printf("Failing at waitpid 1st ELSE if \n");
			errx(1, "pid %d: signal %d", pid, WTERMSIG(x));
		}
		else if (WEXITSTATUS(x) != 0) {
			printf("Failing at waitpid 2nd ELSE if\n");
			errx(1, "pid %d: exit %d", pid, WEXITSTATUS(x));
		}
	}
}


int
main(void)
{
	int pid, pid2, pid3;
	pid = fork();
	pid2 = fork();
	pid3 = fork();
	int nowait = 0;
	if (pid == -1){
		printf("fork failed\n");
	}
	else if(pid == 0){
		printf("hello from child process!\n");
	}
	else{
		dowait(nowait,pid3);
		dowait(nowait, pid2);
		dowait(nowait,pid);
		printf("hello from the parent process ---- %d!\n", pid);

	}


	exit(0);	
}

