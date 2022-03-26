
/*------------------------------------
 * os_file.c
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
#ifdef	_OS_HAVE_L64_
/*================================================================*/
#include <stdio.h>



void *os_fopen(const char *file,const char *attr)
{
	FILE *fp;
	fp = fopen(file, attr);
	return (void *)fp;
}

int os_feof(void *fp);
int os_feof(void *fp)
{
	int i;
	i=feof((FILE *)fp);
	return i;
}

char *os_fgets(void *fp, char *buf, int size);
char *os_fgets(void *fp, char *buf, int size)
{
	char *s;
    if (fp==NULL)
    	return NULL;

    s=fgets(buf, size, (FILE *)fp);
    return s;
}

int os_fread(void *fp, void *buf, int size);
int os_fread(void *fp, void *buf, int size)
{
	int len;
    if (fp==NULL)
    	return -1;

    len=fread(buf, 1, size, (FILE *)fp);
    return len;
}

int os_fputs(void *fp, const char *s);
int os_fputs(void *fp, const char *s)
{
	int len;
    if (fp==NULL)
    	return -1;

    len=fputs(s, (FILE *)fp);
    return len;
}

int os_fwrite(void *fp, void *buf, int size);
int os_fwrite(void *fp, void *buf, int size)
{
	int len;
    if (fp==NULL)
    	return -1;

    len=fwrite(buf, 1, size, (FILE *)fp);
    return len;
}

int os_fclose(void *fp);
int os_fclose(void *fp)
{
	int len;
    if (fp==NULL)
    	return -1;

    len=fclose((FILE *)fp);
    return len;
}

#include <errno.h>
int os_ferror_value(void);
int os_ferror_value(void)
{
	return errno;
}

#include <string.h>
char *os_ferror_name(void);
char *os_ferror_name(void)
{
	return strerror(errno);
}
/*================================================================*/
#include <unistd.h>
int os_fseek_set(void *fp, int offset);
int os_fseek_set(void *fp, int offset)
{
	int i;
	i=fseek((FILE *)fp, (long int)offset, SEEK_SET);
	return i;
}

int os_fseek_cur(void *fp, int offset);
int os_fseek_cur(void *fp, int offset)
{
	int i;
	i=fseek((FILE *)fp, (long int)offset, SEEK_CUR);
	return i;
}

int os_fseek_end(void *fp, int offset);
int os_fseek_end(void *fp, int offset)
{
	int i;
	i=fseek((FILE *)fp, (long int)offset, SEEK_END);
	return i;
}

/*================================================================*/
#include <sys/stat.h>
#include "./os_type.h"
dword os_get_file_len(int fd);
dword os_get_file_len(int fd)
{
    struct stat st;
    fstat(fd, &st);
    return (st.st_size);
}

/*================================================================*/
#endif
/* end of os_file.c */
