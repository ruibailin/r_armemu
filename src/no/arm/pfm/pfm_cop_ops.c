
/*------------------------------------
 * pfm_cop_ops.c
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

/*================================================================*/
void pfm_cnt_cop_reg(void);
void pfm_cnt_cop_reg(void)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_COP_REG_TRANS);
#endif
}

void pfm_cnt_cop_data(void);
void pfm_cnt_cop_data(void)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_COP_DATA_PROC);
#endif
}

void pfm_cnt_cop_ld_st(void);
void pfm_cnt_cop_ld_st(void)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_COP_LOAD_STORE);
#endif
}

/*================================================================*/

/* end of pfm_cop_ops.c */
