
/*------------------------------------
 * os_str.c
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
#include <string.h>
void os_strcpy(char *to,const char *from);
void os_strcpy(char *to,const char *from)
{
	strcpy(to,from);
}

int os_strlen(const char *s);
int os_strlen(const char *s)
{
	int i;
	i=strlen(s);
	return i;
}

char *os_strdup (const char *s);
char *os_strdup (const char *s)
{
	char *d;
	d=strdup(s);
	return d;
}
/*================================================================*/
int os_strcmp(const char *s,const char *d);
int os_strcmp(const char *s,const char *d)
{
	int i;
	i=strcmp(s,d);
	return i;
}

int os_strcasecmp(const char *s,const char *d);
int os_strcasecmp(const char *s,const char *d)
{
	int i;
	i=strcasecmp(s,d);
	return i;
}

/*================================================================*/
#include <stdlib.h>
int os_atoi(const char *s);
int os_atoi(const char *s)
{
	int i;
	i=atoi(s);
	return i;
}

/*================================================================*/
#include <ctype.h>
int os_isspace(char c);
int os_isspace(char c)
{
	int i;
	i=isspace((int)c);
	return i;
}

/*================================================================*/
char *os_trim(char *str);
char *os_trim(char *str)
{
    char *s = str;
    char *end;

    /* remove leading spaces/tabs */
    while (os_isspace(*s))
        s++;

    /* check for null string */
    if (*s == '\0')
        return s; /* already zero length */

    /* remove trailing spaces and newlines */
    end = s + strlen(s) - 1;
    while (end != s && os_isspace(*end))
        end--;
    *(end + 1) = '\0';

    return s;
}
/*================================================================*/
#endif
/* end of os_str.c */
