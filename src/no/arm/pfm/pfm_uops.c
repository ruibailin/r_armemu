
/*------------------------------------
 * pfm_uops.c
 * Create:  2021-02-13
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */

#include "../0ctr.h"
/*================================================================*/
#include "pfm_cpu.h"

#include "../uop/uop_dec.h"

/*================================================================*/
void pfm_cnt_uops(struct uop *op);
void pfm_cnt_uops(struct uop *op)
{
#if COUNT_UOPS
        inc_perf_counter(UOP_BASE + op->opcode);
#endif
}
/*================================================================*/

/* end of pfm_uops.c */
