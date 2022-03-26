
/*------------------------------------
 * arm.h
 * Create:  2021-02-20
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
#ifndef SRC_NO_ARM_ARM_H_
#define SRC_NO_ARM_ARM_H_
/*================================================================*/
#include "./uop/uop.h"
extern void arm_ini_all(void);
extern struct uop *arm_proc_entry(struct uop *op);
extern void arm_ini_all_thread(void);
extern void arm_del_all_thread(void);
/*================================================================*/
#endif 
/* end of arm.h */
