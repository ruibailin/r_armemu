
/*------------------------------------
 * isa.h
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
#ifndef NO_ARM_ISA_ISA_H_
#define NO_ARM_ISA_ISA_H_
/*================================================================*/
#include "isa_cpu.h"

/*================================================================*/
//To be compatible to old version
extern enum arm_core get_core();
extern enum arm_instruction_set get_isa();
/*================================================================*/
extern bool isa_7_core(void);
extern bool isa_9_core(void);
extern bool isa_9e_core(void);
/*================================================================*/
extern bool isa_v4_iset(void);
extern bool isa_v5_iset(void);
extern bool isa_v5e_iset(void);
extern bool isa_v6_iset(void);
/*================================================================*/
extern void isa_init_cpu(const char *cpu_type);
/*================================================================*/

#endif 
/* end of isa.h */
