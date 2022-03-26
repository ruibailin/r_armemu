
/*------------------------------------
 * pfm_cpu.c
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
/*================================================================*/
#include "pfm_cpu.h"

struct pfm_struct pfm;
/*================================================================*/
//To be compatible to old version
void add_to_perf_counter(enum perf_counter_type counter, int add);
void add_to_perf_counter(enum perf_counter_type counter, int add)
{
    if (counter < MAX_PERF_COUNTER)
        pfm.perf_counters.count[counter] += add;
}

void inc_perf_counter(enum perf_counter_type counter);
void inc_perf_counter(enum perf_counter_type counter)
{
    add_to_perf_counter(counter, 1);
}

/*================================================================*/
#include "../cpu/cpu.h"
#include "../isa/isa.h"
#include "../uop/uop.h"

/*================================================================*/
int get_cycle_count(void);
int get_cycle_count(void)
{
#if COUNT_CYCLES
    return pfm.perf_counters.count[CYCLE_COUNT];
#else
    return 0;
#endif
}

int get_instruction_count(void);
int get_instruction_count(void)
{
    return pfm.perf_counters.count[INS_COUNT];
}

/*================================================================*/
void pfm_cnt_ins(void);
void pfm_cnt_ins(void)
{
    inc_perf_counter(INS_COUNT);
}

void pfm_cnt_exp(void);
void pfm_cnt_exp(void)
{
    inc_perf_counter(EXCEPTIONS);
}

void pfm_cnt_ins_dec(void);
void pfm_cnt_ins_dec(void)
{
    inc_perf_counter(INS_DECODE);
}

void pfm_cyc_inc(void);
void pfm_cyc_inc(void)
{
#if COUNT_CYCLES
        inc_perf_counter(CYCLE_COUNT);
#endif
}

int pfm_get_cyc_count(void);
int pfm_get_cyc_count(void)
{
#if COUNT_CYCLES
    return pfm.perf_counters.count[CYCLE_COUNT];
#else
    return 0;
#endif
}

int pfm_get_ins_count(void);
int pfm_get_ins_count(void)
{
    return pfm.perf_counters.count[INS_COUNT];
}

/*================================================================*/
/* end of pfm_cpu.c */
