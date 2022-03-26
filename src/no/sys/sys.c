
/*------------------------------------
 * sys.c
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
/*================================================================*/
#include "../1imp.h"

#include "sys_blk.h"
#include "sys_bug.h"
#include "sys_con.h"
#include "sys_dis.h"
#include "sys_inf.h"
#include "sys_pic.h"
#include "sys_pit.h"
#include "sys_ram.h"
#include "sys_rom.h"

/*================================================================*/
void sys_ini_all(void);
void sys_ini_all()
{
	sys_ini_blk();
	sys_ini_bug();
	sys_ini_con();
	sys_ini_dis();
	sys_ini_inf();
	sys_ini_pic();
	sys_ini_pit();
	sys_ini_ram();
	sys_ini_rom();
}

void sys_ini_all_thread(void);
void sys_ini_all_thread()
{
	sys_ini_con_thread();
	sys_ini_dis_thread();
}

int sys_proc_entry(void);
int sys_proc_entry(void)
{
	int i;
	sys_dis_proc_entry();
	i = sys_con_proc_entry();
	return i;			///if i==1, exit main loop
}


void sys_del_all_thread(void);
void sys_del_all_thread()
{
	sys_del_con_thread();
	sys_del_dis_thread();
}

void sys_del_all(void);
void sys_del_all()
{
	sys_del_blk();
	sys_del_bug();
	sys_del_con();
	sys_del_dis();
	sys_del_inf();
	sys_del_pic();
	sys_del_pit();
	sys_del_ram();
	sys_del_rom();
}
/*================================================================*/
/* end of sys.c */
