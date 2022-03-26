
/*------------------------------------
 * sys_pit.c
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
/*================================================================*/
#include "../1imp.h"
#if ARM_32_BUS
#include "../b32/2exp.h"
#else
#include "../b64/2exp.h"
#endif
#include "../util/2exp.h"


static struct pit_t {
    reg_t status;
    dword clear;			//PIT_CLEAR         (PIT_REGS_BASE + 4)
    dword clear_int;		//PIT_CLEAR_INT     (PIT_REGS_BASE + 8)
    reg_t curr_interval;

    dword oneshot;			//PIT_START_ONESHOT (PIT_REGS_BASE + 16)
    bool tmp;
    bool periodic;			//PIT_START_PERIODIC (PIT_REGS_BASE + 20)
    void *curr_timer;
    void *mutex;

} pit;

/*================================================================*/
#include "sys_pic.h"
#define PIT_STATUS_ACTIVE    0x1
#define PIT_STATUS_INT_PEND  0x2

static u32 pit_callback(u32 interval, void *param);
static u32 pit_callback(u32 interval, void *param)
{
    SYS_TRACE(5, "pit_callback: interval %d\n", interval);

    // make sure there is still an active timer
    if (pit.curr_timer == NULL)
        goto exit;

    // level trigger an interrupt
    pit.status |= PIT_STATUS_INT_PEND;
    pic_assert_level(INT_PIT);

    if (!pit.periodic) {
        // cancel the timer
        os_sdl1_del_timer(pit.curr_timer);
        pit.curr_timer = NULL;
        pit.status &= ~PIT_STATUS_ACTIVE;
    }

exit:
    return interval;
}

/*================================================================*/
#define PIT_STATUS        (PIT_REGS_BASE + 0) // status bit
#define PIT_CLEAR         (PIT_REGS_BASE + 4) // a nonzero write clears any pending timer
#define PIT_CLEAR_INT     (PIT_REGS_BASE + 8) // a nonzero write clears the pending interrupt
#define PIT_INTERVAL      (PIT_REGS_BASE + 12) // set the countdown interval, and what the interval is reset to if periodic
#define PIT_START_ONESHOT (PIT_REGS_BASE + 16) // a nonzero write starts a oneshot countdown
#define PIT_START_PERIODIC (PIT_REGS_BASE + 20) // a nonzero write starts a periodic countdown

static dword pit_regs_get_put(armaddr_t address, dword data, int size, int put);
static dword pit_regs_get_put(armaddr_t address, dword data, int size, int put)
{
    dword val = 0;

    SYS_TRACE(5, "sys: pit_regs_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    if (size < 4)
        return 0; /* only word accesses supported */


    switch (address) {
        case PIT_STATUS: // status bit
            val = pit.status;
            break;
        case PIT_INTERVAL:
            if (put && data != 0) {
                pit.curr_interval = data;
            }
            val = pit.curr_interval;
            break;
        case PIT_START_ONESHOT:
            if (put && data != 0) {
                pit.periodic = FALSE;
                goto set_timer;
            }
            break;
        case PIT_START_PERIODIC:
            if (put && data != 0) {
                pit.periodic = TRUE;
                goto set_timer;
            }
            break;

set_timer:
            // clear any old timer
            if (pit.curr_timer != NULL) {
               os_sdl1_del_timer(pit.curr_timer);
                pit.curr_timer = NULL;
            }
            pit.curr_timer = os_sdl1_add_timer(pit.curr_interval, &pit_callback);
            pit.status |= PIT_STATUS_ACTIVE;
            break;
        case PIT_CLEAR:
            if (put && data != 0 && pit.curr_timer != NULL) {
                os_sdl1_del_timer(pit.curr_timer);
                pit.curr_timer = NULL;
                pit.status &= ~PIT_STATUS_ACTIVE;
            }
            break;
        case PIT_CLEAR_INT:
            if (put && data != 0) {
                pit.status &= ~PIT_STATUS_INT_PEND;
                pic_deassert_level(INT_PIT);
            }
            break;
    }

    return val;
}

/*================================================================*/
void sys_ini_pit(void);
void sys_ini_pit(void)
{
	int host_mem_len;
	void *host_mem_addr;

    os_memclr(&pit, sizeof(pit));

	host_mem_len = sizeof(pit);
	host_mem_addr = &pit;
    // install the pic register handlers
    bus_set_func(PIT_REGS_BASE,pit_regs_get_put,NULL);
	bus_set_host_base_addr(PIT_REGS_BASE, host_mem_addr);
	bus_bat_set_func(PIT_REGS_BASE,host_mem_len);
}

void sys_del_pit(void);
void sys_del_pit(void)
{

}

/*================================================================*/

/* end of sys_pit.c */
