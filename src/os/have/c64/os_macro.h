
/*------------------------------------
 * os_macro.h
 * Create:  2021-02-04
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
#ifndef OS_HAVE_C64_OS_MACRO_H_
#define OS_HAVE_C64_OS_MACRO_H_
/*================================================================*/

#ifndef    MIN
#define    MIN(a,b)    (((a)<(b))?(a):(b))
#endif

#ifndef    MAX
#define    MAX(a,b)    (((a)<(b))?(b):(a))
#endif

/*================================================================*/
#endif
#endif 
/* end of os_macro.h */
