
/*------------------------------------
 * pfm_cpu.c
 * Create:  2021-02-14
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
#include "../1imp.h"
#include "pfm_cpu.h"
#include "../uop/uop.h"
dword pfm_speed_timer(dword interval);
dword pfm_speed_timer(dword interval)
{
    static struct perf_counters old_perf_counters;
    struct perf_counters delta_perf_counter;
    int i;

    for (i=0; i<MAX_PERF_COUNTER; i++) {
        delta_perf_counter.count[i] = pfm.perf_counters.count[i] - old_perf_counters.count[i];
        old_perf_counters.count[i] = pfm.perf_counters.count[i];
    }

#if COUNT_CYCLES
    os_printf("%d cycles/sec, ",
           delta_perf_counter.count[CYCLE_COUNT]);
#endif
    os_printf("%7d ins/sec, %7d ins decodes/sec, exceptions/sec %5d\n",
           delta_perf_counter.count[INS_COUNT],
           delta_perf_counter.count[INS_DECODE],
           delta_perf_counter.count[EXCEPTIONS]);
#if COUNT_MMU_OPS
    os_printf("%7d slow mmu translates/sec, %7d ins fetches, %7d mmu reads, %7d mmu writes, %7d fastpath, %7d slowpath\n",
           delta_perf_counter.count[MMU_SLOW_TRANSLATE],
           delta_perf_counter.count[MMU_INS_FETCH],
           delta_perf_counter.count[MMU_READ],
           delta_perf_counter.count[MMU_WRITE],
           delta_perf_counter.count[MMU_FASTPATH],
           delta_perf_counter.count[MMU_SLOWPATH]);
#endif
#if COUNT_ARM_OPS
    os_printf("\tSC %d NOP %d L %d S %d DP %d MUL %d B %d MISC %d\n",
           delta_perf_counter.count[OP_SKIPPED_CONDITION],
           delta_perf_counter.count[OP_NOP],
           delta_perf_counter.count[OP_LOAD],
           delta_perf_counter.count[OP_STORE],
           delta_perf_counter.count[OP_DATA_PROC],
           delta_perf_counter.count[OP_MUL],
           delta_perf_counter.count[OP_BRANCH],
           delta_perf_counter.count[OP_MISC]);
#endif

#if COUNT_UOPS
    for (i=0; i < MAX_UOP_OPCODE; i++) {
        os_printf("\tuop opcode %3d (%s): %d\n", i, uop_opcode_to_str(i), delta_perf_counter.count[UOP_BASE + i]);
    }
#endif
#if COUNT_ARITH_UOPS
    for (i=0; i < 16; i++) {
        os_printf("\tuop arith opcode %2d (%s): %d\n", i, dp_op_to_str(i), delta_perf_counter.count[UOP_ARITH_OPCODE + i]);
    }
#endif
    return interval;
}

/*================================================================*/
dword speedtimer(dword interval, void *param);
dword speedtimer(dword interval, void *param)
{
    pfm_speed_timer(interval);
	return interval;
}

/*================================================================*/
/* end of pfm_cpu.c */
