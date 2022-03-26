
/*------------------------------------
 * cop.h
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

#ifndef NO_ARM_COP_COP_H_
#define NO_ARM_COP_COP_H_
/*================================================================*/
#include "cop_cpu.h"

extern void cop_init_cop(int cp_num, struct arm_coprocessor *coproc);
extern void cop_init_cop15(void);
/*================================================================*/
//To be compatible to old version
extern void install_coprocessor(int cp_num, struct arm_coprocessor *coproc);
extern void install_cp15(void);
/*================================================================*/
#endif
/* end of cop.h */
