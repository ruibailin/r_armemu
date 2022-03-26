
/*------------------------------------
 * pfm_cpu.h
 * Create:  2021-02-12
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
#ifndef NO_ARM_PFM_PFM_CPU_H_
#define NO_ARM_PFM_PFM_CPU_H_
/*================================================================*/
#include "../uop/uop_opc.h"

/* performance counters, including current cycle and instruction count */
/* to disable for speed purposes, move MAX_PERF_COUNTER up in the enum */
enum perf_counter_type {
    INS_COUNT,

    EXCEPTIONS,

    INS_DECODE,

#if COUNT_MMU_OPS
    MMU_READ,
    MMU_WRITE,
    MMU_INS_FETCH,
    MMU_FASTPATH,
    MMU_SLOWPATH,
    MMU_SLOW_TRANSLATE,
#endif

#if COUNT_CYCLES
    CYCLE_COUNT,
#endif
#if COUNT_ARM_OPS
    OP_SKIPPED_CONDITION,
    OP_NOP,
    OP_LOAD,
    OP_STORE,
    OP_DATA_PROC,
    OP_MUL,
    OP_BRANCH,
    OP_COP_REG_TRANS,
    OP_COP_DATA_PROC,
    OP_COP_LOAD_STORE,
    OP_MISC,
#endif

#if COUNT_UOPS
    UOP_BASE,
    UOP_TOP = UOP_BASE + MAX_UOP_OPCODE,
#endif

#if COUNT_ARITH_UOPS
    UOP_ARITH_OPCODE,
    UOP_ARITH_OPCODE_TOP = UOP_ARITH_OPCODE + 16,
#endif

    // setting this lower will tend to make the compiler optimize away calls to add_to_perf_count
    MAX_PERF_COUNTER,
};

/*================================================================*/
struct perf_counters {
    int count[MAX_PERF_COUNTER];
};

//Just for easily to change code.   cpu.perf_counter -> pfm.perf_counter
struct pfm_struct {
    // tracks emulator performance including cycle count and instruction count
    struct perf_counters perf_counters;
};

extern struct pfm_struct pfm;
extern void add_to_perf_counter(enum perf_counter_type counter, int add);
extern void inc_perf_counter(enum perf_counter_type counter);
/*================================================================*/
#endif 
/* end of pfm_cpu.h */
