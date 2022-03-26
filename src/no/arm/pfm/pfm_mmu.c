
/*------------------------------------
 * pfm_mmu.c
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
void pfm_mmu_ops(enum perf_counter_type counter);
void pfm_mmu_ops(enum perf_counter_type counter)
{
#if COUNT_MMU_OPS
    inc_perf_counter(counter);
#endif
}

void pfm_mmu_read();
void pfm_mmu_read()
{
#if COUNT_MMU_OPS
	inc_perf_counter(MMU_READ);
#endif
}

void pfm_mmu_write();
void pfm_mmu_write()
{
#if COUNT_MMU_OPS
	inc_perf_counter(MMU_WRITE);
#endif
}

void pfm_mmu_ins();
void pfm_mmu_ins()
{
#if COUNT_MMU_OPS
	inc_perf_counter(MMU_INS_FETCH);
#endif
}

void pfm_mmu_fast();
void pfm_mmu_fast()
{
#if COUNT_MMU_OPS
	inc_perf_counter(MMU_FASTPATH);
#endif
}

void pfm_mmu_slow();
void pfm_mmu_slow()
{
#if COUNT_MMU_OPS
	inc_perf_counter(MMU_SLOWPATH);
#endif
}

void pfm_mmu_tran();
void pfm_mmu_tran()
{
#if COUNT_MMU_OPS
	inc_perf_counter(MMU_SLOW_TRANSLATE);
#endif
}

/*================================================================*/

/* end of pfm_mmu.c */
