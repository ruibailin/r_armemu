
/*------------------------------------
 * isa_cpu.h
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

#include "../0ctr.h"
#ifndef NO_ARM_ISA_CPU_H_
#define NO_ARM_ISA_CPU_H_
/*================================================================*/
enum arm_core {
    ARM7 = 0,
    ARM9,
    ARM9e,
};

enum arm_instruction_set {
    ARM_V4 = 0,
    ARM_V5,
    ARM_V5e,
    ARM_V6
};

/*================================================================*/
struct isa_struct {

	// cpu config
    enum arm_core core;

    enum arm_instruction_set isa;
};

//Just for easily to change code.   cpu.isa -> isa.isa
extern struct isa_struct isa;
/*================================================================*/
#endif 
/* end of isa_cpu.h */
