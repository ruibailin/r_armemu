
/*------------------------------------
 * os_stream.c
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
#ifdef	_OS_HAVE_C64_

/*================================================================*/
#include <fcntl.h>
int os_open(const char *file,int flag);
int os_open(const char *file,int flag)
{
	int fd;
	fd=open(file,flag);
	return fd;
}

#include <unistd.h>
int os_lseek_set(int fd, int offset);
int os_lseek_set(int fd, int offset)
{
	int i;
	i=lseek(fd, offset, SEEK_SET);
	return i;
}

int os_lseek_cur(int fd, int offset);
int os_lseek_cur(int fd, int offset)
{
	int i;
	i=lseek(fd, offset, SEEK_CUR);
	return i;
}

int os_lseek_end(int fd, int offset);
int os_lseek_end(int fd, int offset)
{
	int i;
	i=lseek(fd, offset, SEEK_END);
	return i;
}
/*================================================================*/
#include <unistd.h>
int os_read(int fd, void *buf, int size);
int os_read(int fd, void *buf, int size)
{
	int i;
	i=read(fd, buf, size);
	return i;
}

int os_write(int fd, void *buf, int size);
int os_write(int fd, void *buf, int size)
{
	int i;
	i=write(fd, buf, size);
	return i;
}


/*================================================================*/
#endif
/* end of os_stream.c */
