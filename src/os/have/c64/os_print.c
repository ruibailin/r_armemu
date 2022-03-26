
/*------------------------------------
 * os_print.c
 * Create:  2021-01-30
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */

#include "0ctr.h"
#ifdef	_OS_HAVE_C64_
/*================================================================*/
#include <stdio.h>
void os_puts(const char *s);
void os_puts(const char *s)
{
	puts(s);
}

void os_printf(const char *fmt, ...);
void os_printf(const char *fmt, ...)
{
	printf(fmt);
}

void os_print_addr(void *add);
void os_print_addr(void *add)
{
	unsigned long i;
	i=(unsigned long)add;
	printf("0x%lx \n",i);
}

/*================================================================*/
#endif
/* end of os_print.c */
