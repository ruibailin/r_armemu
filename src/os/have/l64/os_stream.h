
/*------------------------------------
 * os_stream.h
 * Create:  2021-02-03
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

#ifndef OS_HAVE_L64_OS_STREAM_H_
#define OS_HAVE_L64_OS_STREAM_H_
/*================================================================*/

extern int os_open(const char *file,int flag);
extern int os_lseek_set(int fd, int offset);
extern int os_lseek_cur(int fd, int offset);
extern int os_lseek_end(int fd, int offset);
extern int os_read(int fd, void *buf, int size);
extern int os_write(int fd, void *buf, int size);
extern int os_close(int fd);
/*================================================================*/
#endif
#endif 
/* end of os_stream.h */
