
/*------------------------------------
 * no_main.c
 * Create:  2021-01-24
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
#include "1imp.h"
/*================================================================*/
#include "./arm/arm.h"
#include "./b32/b32.h"
#include "./cfg/cfg.h"
#include "./sys/sys.h"
/*================================================================*/
void main_loop_no_thread(void);
void main_loop_no_thread(void)
{
	int i;
	struct uop *op,*n;
	op = NULL;
	i=0;
	for(;;)
	{
		sys_dis_proc_entry();
		n = arm_proc_entry(op);
		op = n;
	}
}

#include "./sys/sys_con.h"
void main_loop_with_thread(void);
void main_loop_with_thread(void)
{
	int i;
	for(;;)
	{
    	i=sys_con_thread_stop();
    	if(i)
    		break;
	}
}

/*================================================================*/
#define USE_SDL_THREAD			1
int no_main(int argc, char **argv);
int no_main(int argc, char **argv)
{
	arm_ini_all();
	b32_ini_all();
	cfg_ini_all();
	sys_ini_all();
#if USE_SDL_THREAD
	sys_ini_all_thread();
	arm_ini_all_thread();
	main_loop_with_thread();
#else
	main_loop_no_thread();
#endif

#if USE_SDL_THREAD
	sys_del_all_thread();
	arm_del_all_thread();
#endif
	sys_del_all();
    return 1;
}

/*================================================================*/
/* end of no_main.c */
