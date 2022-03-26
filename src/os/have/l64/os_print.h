
/*------------------------------------
 * os_print.h
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
#ifndef OS_HAVE_L64_OS_PRINT_H_
#define OS_HAVE_L64_OS_PRINT_H_
/*================================================================*/
extern void os_puts(const char *s);
extern void os_printf(const char *fmt, ...);
extern void os_print_addr(void *add);

#define os_print(x...)  \
							{\
								os_printf("\n\033[47;30mPrint:"); \
								os_printf(x); \
								os_printf("\n\033[0m");\
							} while(0)

#define os_debug(x...)  \
							{ \
								os_printf("\n\033[47;32mDebug:"); \
								os_printf(x); \
								os_printf("\n\033[0m");\
							} while(0)
#define os_alarm(x...)   \
							{ \
								os_printf("\n\033[47;33mAlarm:"); \
								os_printf(x); \
								os_printf("\n\033[0m"); \
							} while(0) \

#define os_panic(x...)\
							{ \
								os_printf("\n\033[47;31mPanic:"); \
								os_printf(x); \
								os_printf("\n\033[0m");\
							} while(0)


/*================================================================*/
#endif
#endif 
/* end of os_print.h */
