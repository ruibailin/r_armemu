
/*------------------------------------
 * sys_pic.h
 * Create:  2021-02-04
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
#ifndef NO_SYS_SYS_PIC_H_
#define NO_SYS_SYS_PIC_H_
/*================================================================*/
extern int pic_assert_level(int vector);
extern int pic_deassert_level(int vector);

extern void sys_ini_pic(void);
extern void sys_del_pic(void);
/*================================================================*/
#endif

/* end of sys_pic.h */
