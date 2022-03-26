
/*------------------------------------
 * cdt_cpu.h
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

#include "../0ctr.h"

#ifndef NO_ARM_CDT_CDT_CPU_H_
#define NO_ARM_CDT_CDT_CPU_H_
/*================================================================*/

struct cdt_struct {
    // truth table of the arm conditions
    unsigned short condition_table[16];
};

//Just for easily to change code.   cpu.condition_table -> cdt.condition_table
extern struct cdt_struct cdt;
/*================================================================*/
#endif 
/* end of cdt_cpu.h */
