
/*------------------------------------
 * os_atomic.c
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
#if __OS_CPU_X86__

/* defined in atomic_asm.S */

#endif

#if __OS_CPU_PPC__

/* defined in atomic_asm.S */

#endif

#if __OS_CPU_SPARC__
#error implement SPARC atomic_* ops
#endif

/*================================================================*/

/* end of os_atomic.c */
