
/*------------------------------------
 * os_file.h
 * Create:  2021-01-31
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
#ifndef OS_HAVE_L64_OS_FILE_H_
#define OS_HAVE_L64_OS_FILE_H_
/*================================================================*/
extern void *os_fopen(const char *file, const char *attr);
extern int os_feof(void *fp);
extern char *os_fgets(void *fp, char *buf, int size);
extern int os_fputs(void *fp, const char *s);
extern int os_fread(void *fp, void *buf, int size);
extern int os_fwrite(void *fp, void *buf, int size);
extern int os_fclose(void *fp);
extern int os_ferror_value(void);
extern char *os_ferror_name(void);

extern int os_fseek_set(void *fp, int offset);
extern int os_fseek_cur(void *fp, int offset);
extern int os_fseek_end(void *fp, int offset);

extern int os_lseek_set(int fd, int offset);
extern int os_lseek_cur(int fd, int offset);
extern int os_lseek_end(int fd, int offset);
extern int os_read(int fd, void *buf, int size);
extern int os_write(int fd, void *buf, int size);

#include "./os_type.h"
extern dword os_get_file_len(int fd);
/*================================================================*/
#endif
#endif 
/* end of os_file.h */
