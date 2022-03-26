
/*------------------------------------
 * arm.c
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

#include "0ctr.h"
#include "1imp.h"
/*================================================================*/
#include "./cdp/cdp.h"
#include "./cdt/cdt.h"
#include "./cop/cop.h"
#include "./cpu/cpu.h"
#include "./isa/isa.h"
#include "./mmu/mmu.h"
#include "./uop/uop.h"
/*================================================================*/
void arm_ini_all(void);
void arm_ini_all(void)
{
	cpu_init_cpu();
	isa_init_cpu("arm926ejs");
	cop_init_cop15();
	cdt_build_table();
	cdp_init_cdp();
	mmu_init_mmu(1);		//with mmu
}


/*================================================================*/
//can be directly called, if don't want to use in a independent thread;
struct uop *arm_proc_entry(struct uop *op);
struct uop *arm_proc_entry(struct uop *op)
{
    struct uop *n;
    process_pending_exceptions();

    /* main dispatch loop */
    n=uop_from_cpu_decode(op);

    return n;
}


//if use SDL thread.
// main console loop
#include "../sys/sys_con.h"
static int arm_thread_entry(void *args);
static int arm_thread_entry(void *args)
{
	int i;
	struct uop *op,*n;
	op = NULL;
	for(;;)
	{
    	i=sys_con_thread_stop();
    	if(i)
    		break;

		n = arm_proc_entry(op);
		op = n;
	}

    return 0;
}

/*================================================================*/
static void *arm_thread_id;
void arm_ini_all_thread(void);
void arm_ini_all_thread(void)
{
	arm_thread_id=os_sdl1_create_thread(&arm_thread_entry);
}


void arm_del_all_thread(void);
void arm_del_all_thread(void)
{
	os_sdl1_destroy_thread(arm_thread_id);
}
/*================================================================*/
/* end of arm.c */
