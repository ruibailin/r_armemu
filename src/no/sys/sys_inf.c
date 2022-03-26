
/*------------------------------------
 * sys_inf.c
 * Create:  2021-02-07
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
#if ARM_32_BUS
#include "../b32/2exp.h"
#else
#include "../b64/2exp.h"
#endif
#include "../util/2exp.h"

/* global system state */
static struct sysinfo_t {
    /* features */
    dword features;

    /* current latched system time */
    qword tv_sec;
    dword tv_usec;
} sysinfo;
/*================================================================*/
#define SYSINFO_FEATURES  (SYSINFO_REGS_BASE + 0)

/* a write to this register latches the current emulator system time, so the next two regs can be read atomically */
#define SYSINFO_TIME_LATCH (SYSINFO_REGS_BASE + 4)
/* gettimeofday() style time values */
#define SYSINFO_TIME_SECS  (SYSINFO_REGS_BASE + 8)
#define SYSINFO_TIME_USECS (SYSINFO_REGS_BASE + 12)

static dword sysinfo_get_put(armaddr_t address, dword data, int size, int put)
{
    switch (address) {
        case SYSINFO_FEATURES:
            return sysinfo.features;
        case SYSINFO_TIME_LATCH:
            if (put) {
            	sysinfo.tv_usec = os_tv_get_usec();
            	sysinfo.tv_sec = os_tv_get_sec();
            }
            break;
        case SYSINFO_TIME_SECS:
            return sysinfo.tv_sec;
        case SYSINFO_TIME_USECS:
            return sysinfo.tv_usec;
    }

    return 0;
}
/*================================================================*/
#define SYS_DISPLAY 0x00000001
#define SYS_CONSOLE 0x00000002
#define SYS_NETWORK 0x00000004
#define SYS_BLOCKDEV 0x00000008

void sys_ini_inf(void);
void sys_ini_inf(void)
{
	int host_mem_len;
	void *host_mem_addr;

	sysinfo.features = SYS_DISPLAY|SYS_CONSOLE|SYS_NETWORK|SYS_BLOCKDEV;
	sysinfo.tv_usec = os_tv_get_usec();
	sysinfo.tv_sec = os_tv_get_sec();

//    host_mem_len = SYSINFO_TIME_USECS + 4 -SYSINFO_REGS_BASE;
//    host_mem_add=os_malloc(host_mem_len);
	host_mem_len = sizeof(sysinfo);
	host_mem_addr = &sysinfo;
    // install the pic register handlers
	bus_set_func(SYSINFO_REGS_BASE,sysinfo_get_put,NULL);
	//we can use &sysinfo directly as host memory address
	bus_set_host_base_addr(SYSINFO_REGS_BASE,host_mem_addr);
	bus_bat_set_func(SYSINFO_REGS_BASE, host_mem_len);
}

void sys_del_inf(void);
void sys_del_inf(void)
{
//	void *host_mem_addr;
//	host_mem_addr = bus_get_mem_ptr(SYSINFO_REGS_BASE);
//	os_free(host_mem_addr);
}
/*================================================================*/

/* end of sys_inf.c */
