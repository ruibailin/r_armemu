
/*------------------------------------
 * cpu_cop.h
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
#ifndef NO_ARM_COP_COP_CPU_H_
#define NO_ARM_COP_COP_CPU_H_
/*================================================================*/
#include "../1imp.h"

struct arm_coprocessor {
    int installed;
    void *data;

    // the callback routines
    void (*reg_transfer)(dword ins, void *data);
    void (*double_reg_transfer)(dword ins, void *data);
    void (*data_processing)(dword ins, void *data);
    void (*load_store)(dword ins, void *data);
};

/*================================================================*/
struct cop_struct {
    // routines for the arm's 16 coprocessor slots
    struct arm_coprocessor coproc[16];
};

//Just for easily to change code.   cpu.coproc -> cop.coproc
extern struct cop_struct cop;
/*================================================================*/
#endif 
/* end of cpu_cop.h */
