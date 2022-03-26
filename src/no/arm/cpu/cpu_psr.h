
/*------------------------------------
 * cpu_psr.h
 * Create:  2021-02-10
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */


#ifndef NO_ARM_CPU_CPU_PSR_H_
#define NO_ARM_CPU_CPU_PSR_H_
/*================================================================*/
#include "../1imp.h"

// CPSR bits
#define PSR_MODE_MASK 0x1f
#define PSR_MODE_user 0x10
#define PSR_MODE_fiq  0x11
#define PSR_MODE_irq  0x12
#define PSR_MODE_svc  0x13
#define PSR_MODE_abt  0x17
#define PSR_MODE_und  0x1b
#define PSR_MODE_sys  0x1f

#define PSR_IRQ_MASK  0x80
#define PSR_FIQ_MASK  0x40
#define PSR_THUMB     0x20

#define PSR_CC_NEG    0x80000000
#define PSR_CC_ZERO   0x40000000
#define PSR_CC_CARRY  0x20000000
#define PSR_CC_OVL    0x10000000
#define PSR_CC_Q      0x08000000

/*================================================================*/
#endif 
/* end of cpu_psr.h */
