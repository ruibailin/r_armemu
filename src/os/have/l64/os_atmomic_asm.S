/*------------------------------------
 * os_atomic_asm.s
 * Create:  2021-02-21
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */
 
 /*================================================================*/
 .text

/* Some host architectures require underscores in front of all of the symbols */
/*
 * Can't combine the two macros because some assemblers require .globl directives
 * to be on their own line
 */


/* int atomic_add(int *val, int incr) */
.globl atomic_add
atomic_add:
	movl		%esi,%eax		/* incr -> eax */
	lock
	xaddl	%eax,(%rsi)		/* atomically add incr to *val */
							/* old value is in eax */
	ret

/* int atomic_and(int *val, int incr) */
.globl atomic_and
atomic_and:
1:
	movl		(%rdi),%eax		/* *val -> temp2 */
	movl		%esi,%ecx		/* incr -> temp */
	andl		%eax,%ecx		/* temp &= temp2 */

	lock
	cmpxchgl	%ecx,(%rdi) /*,%eax*/	/* if temp2 is same as *val, temp -> *val */
							/* old value is in eax */

	jnz		1b			/* if it didn't succeed, try again */
	ret

/* int atomic_or(int *val, int incr) */
.globl atomic_or
atomic_or:
1:
	movl		(%rdi),%eax		/* *val -> temp2 */
	movl		%esi,%ecx		/* incr -> temp */
	orl		%eax,%ecx		/* temp |= temp2 */

	lock
	cmpxchgl	%ecx,(%rdi) /*,%eax*/	/* if temp2 is same as *val, temp -> *val */
							/* old value is in eax */

	jnz			1b			/* if it didn't succeed, try again */
	ret

/* int atomic_set(int *val, int set_to) */
.globl atomic_set
atomic_set:
	movl		%esi,%eax		/* set_to -> eax */
	xchg		%eax,(%rdi)		/* *val += set_to */
							/* old value is in eax */
	ret

/* int test_and_set(int *val, int set_to, int test_val) */
.globl test_and_set
test_and_set:
	movl		%edx,%eax		/* test_val -> eax */

	lock
	cmpxchgl	%esi,(%rdi) /*,%eax*/	/* if test_val is same as *val, set_to -> *val */
							/* old value is in eax */
	ret
 /*================================================================*/
 
/* end of os_atomic_asm.s */
