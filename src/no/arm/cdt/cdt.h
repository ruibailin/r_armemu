
/*------------------------------------
 * cdt.h
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
#ifndef NO_ARM_CDT_CDT_H_
#define NO_ARM_CDT_CDT_H_
/*================================================================*/
#include "../1imp.h"
#include "cdt_cpu.h"

/*================================================================*/
extern bool cdt_check_table(byte condition);
extern int cdt_build_table(void);
/*================================================================*/
//To be compatible to old version
extern bool check_condition(byte condition);
extern int build_condition_table(void);
/*================================================================*/
#endif 
/* end of cdt.h */
