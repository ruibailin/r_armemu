
/*------------------------------------
 * mmu_reg.h
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
#ifndef NO_ARM_MMU_REG_H_
#define NO_ARM_MMU_REG_H_
/*================================================================*/
/* getting and setting internal mmu registers */
enum mmu_registers {
    MMU_TRANS_TABLE_REG,
    MMU_DOMAIN_ACCESS_CONTROL_REG,
    MMU_FAULT_STATUS_REG,
    MMU_FAULT_ADDRESS_REG,
};
/*================================================================*/
#endif 
/* end of mmu_reg.h */
