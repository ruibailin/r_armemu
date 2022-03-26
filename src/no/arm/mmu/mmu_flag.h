
/*------------------------------------
 * mmu_flag.h
 * Create:  2021-02-11
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
#ifndef NO_ARM_MMU_FLAG_H_
#define NO_ARM_MMU_FLAG_H_
/*================================================================*/
/* mmu flags (cr1 flags in cp15) */
#define MMU_ENABLED_FLAG          (1<<0)
#define MMU_ALIGNMENT_FAULT_FLAG  (1<<1)
#define MMU_SYS_PROTECTION_FLAG   (1<<8)
#define MMU_ROM_PROTECTION_FLAG   (1<<9)
#define MMU_FLAG_MASK (1 | (1<<1) | (1<<8) | (1<<9))

/*================================================================*/
#endif 
/* end of mmu_flag.h */
