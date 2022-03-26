
/*------------------------------------
 * sys.h
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
#ifndef SRC_NO_SYS_SYS_H_
#define SRC_NO_SYS_SYS_H_
/*================================================================*/
extern void sys_ini_all(void);
extern void sys_del_all(void);

/*================================================================*/
//if use thread;
extern void sys_ini_all_thread(void);
extern void sys_del_all_thread(void);
//if want to directly call procedure;
extern int sys_proc_entry(void);


//if want to directly call console and display functions.
extern int sys_con_proc_entry(void);
extern void sys_ini_con_thread(void);
extern void sys_del_con_thread(void);
extern int  sys_con_thread_stop(void);

extern void sys_dis_proc_entry(void);
extern void sys_ini_dis_thread(void);
extern void sys_del_dis_thread(void);

/*================================================================*/
#endif 
/* end of sys.h */
