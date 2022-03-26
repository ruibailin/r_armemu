
/*------------------------------------
 * sys_con.h
 * Create:  2021-02-19
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
#ifndef SRC_NO_SYS_SYS_CON_H_
#define SRC_NO_SYS_SYS_CON_H_
/*================================================================*/
extern int sys_con_proc_entry(void);

extern void sys_ini_con(void);
extern void sys_del_con(void);

extern void sys_ini_con_thread(void);
extern void sys_del_con_thread(void);
extern int  sys_con_thread_stop(void);
/*================================================================*/
#endif 
/* end of sys_con.h */
