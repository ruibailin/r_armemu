
/*------------------------------------
 * cpu_data.h
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

#include "0ctr.h"

#ifndef NO_ARM_CPU_CPU_DATA_H_
#define NO_ARM_CPU_CPU_DATA_H_
/*================================================================*/
#include "../1imp.h"
#include "../uop/uop_dec.h"

struct cpu_struct {
    // regs
    reg_t pc;           // the real pc
    struct uop *cp_pc;  // pointer to the current op
    reg_t cpsr;
    reg_t r[16];        // contains a "fake" pc made to look as if it's ahead of the current pc
    reg_t spsr;
    bool r15_dirty;     // if we wrote into r[15] in the last instruction

    // pending interrupts and mode changes
    volatile int pending_exceptions;
    reg_t old_cpsr; // in case of a mode switch, we store the old mode
    armaddr_t exception_base; // 0 or 0xffff0000 on cpus that support it

    // banked_regs
    reg_t usr_regs_low[5]; // non-fiq r8-r12
    reg_t usr_regs[2];     // sp, lr
    reg_t irq_regs[3];     // sp, lr, spsr
    reg_t svc_regs[3];     //     "
    reg_t abt_regs[3];     //     "
    reg_t und_regs[3];     //     "
    reg_t fiq_regs[8];     // r8-r12, sp, lr, spsr
};

extern struct cpu_struct cpu;
/*================================================================*/
#endif 
/* end of cpu_data.h */
