
/*------------------------------------
 * os_time.c
 * Create:  2021-02-19
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
#ifdef	_OS_HAVE_L64_
/*================================================================*/
#include "os_type.h"
#include <sys/time.h>

qword os_tv_get_sec(void);
qword os_tv_get_sec()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return (qword)current_time.tv_sec;
}

dword os_tv_get_usec(void);
dword os_tv_get_usec()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return (dword)current_time.tv_usec;
}

/*================================================================*/
#endif
/* end of os_time.c */
