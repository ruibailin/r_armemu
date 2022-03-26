
/*------------------------------------
 * 2exp.h
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
#ifdef	_OS_HAVE_
#ifndef OS_HAVE_2EXP_H_
#define OS_HAVE_2EXP_H_
/*================================================================*/
#ifdef	_OS_HAVE_C32_			//cygwin32
#include "./c32/2exp.h"
#endif

#ifdef	_OS_HAVE_C64_			//cygwin64
#include "./c64/2exp.h"
#endif

#ifdef	_OS_HAVE_L32_			//linux 32
#include "./l32/2exp.h"
#endif

#ifdef	_OS_HAVE_L64_			//linux 64
#include "./l64/2exp.h"
#endif

#ifdef	_OS_HAVE_W32_			//windows 32
#include "./w32/2exp.h"
#endif

#ifdef	_OS_HAVE_W64_			//windows 32
#include "./w64/2exp.h"
#endif


/*================================================================*/
#endif
#endif 
/* end of 2exp.h */
