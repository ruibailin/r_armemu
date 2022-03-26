
/*------------------------------------
 * bus_map.h
 * Create:  2021-02-02
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
#ifndef NO_B32_BUS_MAP_H_
#define NO_B32_BUS_MAP_H_
/*================================================================*/
#include "bus_add.h"

#define MEM_SLOT_SIZE (1<<BUS_BAR_ADD_LINE)

/* memory map of our generic arm system */
// XXX make more dynamic
#define RAM_BASE 	0x0
#define RAM_SIZE 	(MEM_SLOT_SIZE)*1		//4M

#define ROM_BASE	(0xc0000000)
#define ROM_SIZE 	(MEM_SLOT_SIZE)

/* peripherals are all mapped here */
#define PERIPHERAL_BASE   (0xf0000000)

/* system info */
#define SYSINFO_REGS_BASE (PERIPHERAL_BASE)
#define SYSINFO_REGS_SIZE MEM_SLOT_SIZE
/* display */
#define DISPLAY_BASE      (SYSINFO_REGS_BASE + SYSINFO_REGS_SIZE)
#define DISPLAY_SIZE      MEM_SLOT_SIZE

#define DISPLAY_REGS_BASE 		(DISPLAY_BASE + DISPLAY_SIZE)
#define DISPLAY_REGS_SIZE 		MEM_SLOT_SIZE

/* console (keyboard controller */
#define CONSOLE_REGS_BASE (DISPLAY_REGS_BASE + DISPLAY_REGS_SIZE)
#define CONSOLE_REGS_SIZE MEM_SLOT_SIZE

/* programmable timer */
#define PIT_REGS_BASE     (CONSOLE_REGS_BASE + CONSOLE_REGS_SIZE)
#define PIT_REGS_SIZE     MEM_SLOT_SIZE

/* interrupt controller */
#define PIC_REGS_BASE     (PIT_REGS_BASE + PIT_REGS_SIZE)
#define PIC_REGS_SIZE     MEM_SLOT_SIZE

/* debug interface */
#define DEBUG_REGS_BASE (PIC_REGS_BASE + PIC_REGS_SIZE)
#define DEBUG_REGS_SIZE MEM_SLOT_SIZE

/* network interface */
#define NET_REGS_BASE (DEBUG_REGS_BASE + DEBUG_REGS_SIZE)
#define NET_REGS_SIZE MEM_SLOT_SIZE

/* block device interface */
#define BDEV_REGS_BASE (NET_REGS_BASE + NET_REGS_SIZE)
#define BDEV_REGS_SIZE MEM_SLOT_SIZE

/*================================================================*/
#endif 
/* end of bus_map.h */
