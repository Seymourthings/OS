/*opyright (c) 2014
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <test161/test161.h>

#define _PATH_RANDOM   "random:"

/*
 * Caution: OS/161 doesn't provide any way to get this properly from
 * the kernel. The page size is 4K on almost all hardware... but not
 * all. If porting to certain weird machines this will need attention.
 */
#define PAGE_SIZE 4096

////////////////////////////////////////////////////////////
// support code




////////////////////////////////////////////////////////////
// memory checking

/*
 * Fill a page of memory with a test pattern.
 */
static
void
markpage(volatile void *baseptr, unsigned pageoffset)
{
	volatile char *pageptr;
	size_t n, i;
	volatile unsigned long *pl;
	unsigned long val;

	pageptr = baseptr;
	pageptr += (size_t)PAGE_SIZE * pageoffset;

	pl = (volatile unsigned long *)pageptr;
	n = PAGE_SIZE / sizeof(unsigned long);

	for (i=0; i<n; i++) {
		val = ((unsigned long)i ^ (unsigned long)pageoffset);
		pl[i] = val;
	}
}

/*
 * Check a page marked with markblock()
 */
static
int
checkpage(volatile void *baseptr, unsigned pageoffset, bool neednl)
{
	volatile char *pageptr;
	size_t n, i;
	volatile unsigned long *pl;
	unsigned long val;

	pageptr = baseptr;
	pageptr += (size_t)PAGE_SIZE * pageoffset;

	pl = (volatile unsigned long *)pageptr;
	n = PAGE_SIZE / sizeof(unsigned long);

	for (i=0; i<n; i++) {
		val = ((unsigned long)i ^ (unsigned long)pageoffset);
		if (pl[i] != val) {
			if (neednl) {
				tprintf("\n");
			}
			tprintf("FAILED: data mismatch at offset %lu of page "
			       "at 0x%lx: %lu vs. %lu\n",
			       (unsigned long) (i*sizeof(unsigned long)),
			       (unsigned long)(uintptr_t)pl,
			       pl[i], val);
			return -1;
		}
	}

	return 0;
}

/*
 * Light version; touches just the first word of a page.
 */

/*
 * Light version; checks just the first word of a page.
 */

////////////////////////////////////////////////////////////
// error wrapper

static
void *
dosbrk(ssize_t size)
{
	void *p;

	p = sbrk(size);
	if (p == (void *)-1) {
		err(1, "FAILED: sbrk");
	}
	if (p == NULL) {
		errx(1, "FAILED: sbrk returned NULL, which is illegal");
	}
	return p;
}

////////////////////////////////////////////////////////////
// fork a child that segfaults

////////////////////////////////////////////////////////////
// simple allocation

/*
 * Allocate one page, check that it holds data, and leak it.
 */
static
void
test1(void)
{
	void *p;

	tprintf("Allocating a page...\n");
	p = dosbrk(PAGE_SIZE);
	markpage(p, 0);
	if (checkpage(p, 0, false)) {
		errx(1, "FAILED: data corrupt");
	}
	success(TEST161_SUCCESS, SECRET, "/testbin/sbrktest");
}

int main(void){
	test1();
	return 0;

}

