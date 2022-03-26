
/*------------------------------------
 * os_str.h
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
#ifndef OS_HAVE_L64_OS_STR_H_
#define OS_HAVE_L64_OS_STR_H_
/*================================================================*/
extern void os_strcpy(char *to,const char *from);
extern int 	os_strlen(const char *s);
extern char *os_strdup (const char *s);

extern int os_strcmp(const char *s,const char *d);
extern int os_strcasecmp(const char *s,const char *d);

extern int os_atoi(const char *s);

extern int os_isspace(int c);

extern char *os_trim(char *str);
/*================================================================*/
#endif
#endif 
/* end of os_str.h */
