
/*------------------------------------
 * debug.c
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
/*================================================================*/
#include "../1imp.h"

#include "debug.h"

#if DYNAMIC_TRACE_LEVELS
int TRACE_CPU_LEVEL = DEFAULT_TRACE_CPU_LEVEL;
int TRACE_UOP_LEVEL = DEFAULT_TRACE_UOP_LEVEL;
int TRACE_SYS_LEVEL = DEFAULT_TRACE_SYS_LEVEL;
int TRACE_MMU_LEVEL = DEFAULT_TRACE_MMU_LEVEL;
#endif

void _dprintf(const char *fmt, ...)
{
    os_printf(fmt);
}


/*================================================================*/

/* end of debug.c */
