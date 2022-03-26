
/*------------------------------------
 * os_mem.c
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
#include <string.h>

void os_memclr(void *src, int size);
void os_memclr(void *src, int size)
{
	memset(src,0,size);
}

void os_memcpy(void *src, void *dest, int size);
void os_memcpy(void *src, void *dest, int size)
{
	memcpy(src,dest,size);
}

/*================================================================*/
#include <stdlib.h>
void *os_calloc(int cnt, int size);
void *os_calloc(int cnt, int size)
{
	void *p;
	p=calloc(cnt,size);
	return p;
}

void *os_malloc(int size);
void *os_malloc(int size)
{
	void *p;
	p=malloc(size);
	return p;
}

void os_free(void *p);
void os_free(void *p)
{
	if(p==NULL)
		return;
	free(p);
}
/*================================================================*/
#endif
/* end of os_mem.c */
