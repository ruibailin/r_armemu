
/*------------------------------------
 * debug.h
 * Create:  2021-01-24
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
#ifndef NO_NO_DEBUG_H_
#define NO_NO_DEBUG_H_
/*================================================================*/
#if DYNAMIC_TRACE_LEVELS
#define DEFAULT_TRACE_CPU_LEVEL 1
#define DEFAULT_TRACE_UOP_LEVEL 1
#define DEFAULT_TRACE_SYS_LEVEL 1
#define DEFAULT_TRACE_MMU_LEVEL 1
#else
#define TRACE_CPU_LEVEL 1
#define TRACE_UOP_LEVEL 1
#define TRACE_SYS_LEVEL 1
#define TRACE_MMU_LEVEL 1
#endif

#if DYNAMIC_TRACE_LEVELS
extern int TRACE_CPU_LEVEL;
extern int TRACE_UOP_LEVEL;
extern int TRACE_SYS_LEVEL;
extern int TRACE_MMU_LEVEL;
#endif

/* trace */
#if _DEBUG_PRINT_
extern void _dprintf(const char *fmt, ...);
#define CPU_TRACE(level, x...) { if((level) <= TRACE_CPU_LEVEL) _dprintf(x); } while(0)
#define UOP_TRACE(level, x...) { if((level) <= TRACE_UOP_LEVEL) _dprintf(x); } while(0)
#define SYS_TRACE(level, x...) { if((level) <= TRACE_SYS_LEVEL) _dprintf(x); } while(0)
#define MMU_TRACE(level, x...) { if((level) <= TRACE_MMU_LEVEL) _dprintf(x); } while(0)
#else
#define CPU_TRACE(level, x...) { } while(0)
#define UOP_TRACE(level, x...) { } while(0)
#define SYS_TRACE(level, x...) { } while(0)
#define MMU_TRACE(level, x...) { } while(0)
#endif
/*================================================================*/
#endif 
/* end of debug.h */
