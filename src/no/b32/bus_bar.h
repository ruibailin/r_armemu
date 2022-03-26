
/*------------------------------------
 * bus_bar.h
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
#if	ARM_32_BUS
/*================================================================*/
#include "../1imp.h"
#include "bus_add.h"
extern	void bus_set_func(addr_t base,
	    u32 (*get_put)(addr_t addr, u32 data, int size, int put),
	    void* (*get_ptr)(addr_t addr));
extern	void bus_bat_set_func(addr_t base, addr_t len);
extern	void bus_set_host_base_addr(addr_t base, void * h_addr);
extern	void bus_ini_port(void);
/*================================================================*/
//used by any one who needs to read/write the device which implements get_put and get_ptr function.
extern	u32 bus_read_u32(addr_t addr);
extern	u16 bus_read_u16(addr_t addr);
extern	u8 bus_read_u8(addr_t addr);
extern	void bus_write_u32(addr_t addr, u32 data);
extern	void bus_write_u16(addr_t addr, u16 data);
extern	void bus_write_u8(addr_t addr, u8 data);
extern	void *bus_get_mem_ptr(addr_t addr);
extern	void *sys_get_mem_ptr(armaddr_t addr);
/*================================================================*/
#endif
/* end of bus_bar.h */
