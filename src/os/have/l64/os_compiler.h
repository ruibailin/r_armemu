
/*------------------------------------
 * os_compiler.h
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
#ifndef OS_HAVE_L64_OS_COMPILER_H_
#define OS_HAVE_L64_OS_COMPILER_H_
/*================================================================*/
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define __ALWAYS_INLINE __attribute__((always_inline))

#include "os_print.h"
#define ASSERT(x) 	\
    if(unlikely(!(x)))  \
			os_panic("assert failed at %s:%d\n", __FUNCTION__, __LINE__);

/*================================================================*/
#endif
#endif 
/* end of os_compiler.h */
