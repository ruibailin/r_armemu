
/*------------------------------------
 * bus_bar.c
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

#include "../0ctr.h"
#if	ARM_32_BUS
/*================================================================*/
#include "../1imp.h"
#include "bus_add.h"

typedef struct bus_fun {
	addr_t target_base_addr;
	addr_t target_addr_size;
	char * host_base_addr;
    u32 (*get_put)(addr_t addr, u32 data, int size, int put);
    void* (*get_ptr)(addr_t addr);
} bus_func;

#define	 MAX_PORT_NUM	(1<<(32-BUS_BAR_ADD_LINE))
#define  MAX_PORT_MEM	(1<<BUS_BAR_ADD_LINE)
#define  ADDR_TO_PORT(x) ((x) >> BUS_BAR_ADD_LINE)

static	bus_func	 bus_bar[MAX_PORT_NUM];
/*================================================================*/
//define default memory operation. if there is no ROM/RAM, return 0xFF
static u32  def_get_put(addr_t addr, u32 data, int size, int put);
static u32  def_get_put(addr_t addr, u32 data, int size, int put)
{
    switch (size) {
        case 4:
            if (put)
            {
                os_panic("Can't write 32bit at address:%x", addr);
            }
            else
                data = 0xFFFFFFFF;
            break;
        case 2:
            if (put)
            {
                os_panic("Can't write 16bit at address:%x", addr);
            }
            else
                data = 0xFFFF;
            break;
        case 1:
            if (put)
            {
                os_panic("Can't write 8bit at address:%x", addr);
            }
            else
                data = 0xFF;
            break;
    }
    return data;
}

//default is to use host memory as target memory.
static void  *def_get_ptr(addr_t addr);
static void  *def_get_ptr(addr_t addr)
{
	int i;
	addr_t rel;
	i = ADDR_TO_PORT(addr);

	rel = addr-bus_bar[i].target_base_addr;
	if(rel >= bus_bar[i].target_addr_size)
		return NULL;

	return (void *)(bus_bar[i].host_base_addr+rel);
}


/*================================================================*/
void bus_set_func(addr_t base,
	    u32 (*get_put)(addr_t addr, u32 data, int size, int put),
	    void* (*get_ptr)(addr_t addr));
void bus_set_func(addr_t base,
	    u32 (*get_put)(addr_t addr, u32 data, int size, int put),
	    void* (*get_ptr)(addr_t addr))
{
	int i;
	i=ADDR_TO_PORT(base);
	bus_bar[i].target_base_addr=base;
	if(get_ptr != NULL)					//use default function
		bus_bar[i].get_ptr=get_ptr;
	if(get_put != NULL)					//use default function
		bus_bar[i].get_put=get_put;
}

//default is to use host memory as target memory,h_addr is a ram pointer which comes from malloc;
//if use host file as target memory,h_addr is a file pointer which comes from fopen.
void bus_set_host_base_addr(addr_t base, void *h_addr);
void bus_set_host_base_addr(addr_t base, void *h_addr)
{
	int i;
	i = ADDR_TO_PORT(base);

	bus_bar[i].host_base_addr = h_addr;
}


void bus_bat_set_func(addr_t base, addr_t len);
void bus_bat_set_func(addr_t base, addr_t len)
{
	int b,e,i;
	b = ADDR_TO_PORT(base);
	e = ADDR_TO_PORT(base + (len - 1));
	for(i=b+1;i<=e; i++)
	{
		bus_bar[i].host_base_addr=bus_bar[b].host_base_addr;
		bus_bar[i].target_addr_size=len;
		bus_bar[i].get_ptr=bus_bar[b].get_ptr;
		bus_bar[i].get_put=bus_bar[b].get_put;
	}
}

void bus_ini_port(void);
void bus_ini_port()
{
	bus_set_func(0,def_get_put,def_get_ptr);
	bus_set_host_base_addr(0, NULL);
	bus_bat_set_func(0,0);
}
/*================================================================*/
u32 bus_read_u32(addr_t addr);
u32 bus_read_u32(addr_t addr)
{
//	os_printf("read dword:%x \n",addr);
    return bus_bar[ADDR_TO_PORT(addr)].get_put(addr, 0, 4, 0);
}

u16 bus_read_u16(addr_t addr);
u16 bus_read_u16(addr_t addr)
{
//	os_printf("read word:%x \n",addr);
    return bus_bar[ADDR_TO_PORT(addr)].get_put(addr, 0, 2, 0);
}

u8 bus_read_u8(addr_t addr);
u8 bus_read_u8(addr_t addr)
{
//	os_printf("read byte:%x \n",addr);
    return bus_bar[ADDR_TO_PORT(addr)].get_put(addr, 0, 1, 0);
}

void bus_write_u32(addr_t addr, u32 data);
void bus_write_u32(addr_t addr, u32 data)
{
//	os_printf("write dword:%x %x \n",addr,data);
    bus_bar[ADDR_TO_PORT(addr)].get_put(addr, data, 4, 1);
}

void bus_write_u16(addr_t addr, u16 data);
void bus_write_u16(addr_t addr, u16 data)
{
//	os_printf("write word:%x %x \n",addr,data);
    bus_bar[ADDR_TO_PORT(addr)].get_put(addr, data, 2, 1);
}

void bus_write_u8(addr_t addr, u8 data);
void bus_write_u8(addr_t addr, u8 data)
{
	os_printf("write byte:%x %x \n",addr,data);
    bus_bar[ADDR_TO_PORT(addr)].get_put(addr, data, 1, 1);
}

void *bus_get_mem_ptr(addr_t addr);
void *bus_get_mem_ptr(addr_t addr)
{
    return bus_bar[ADDR_TO_PORT(addr)].get_ptr(addr);
}

void *sys_get_mem_ptr(armaddr_t addr);
void *sys_get_mem_ptr(armaddr_t addr)
{
	return bus_get_mem_ptr(addr);
}
/*================================================================*/
#endif
/* end of bus_bar.c */
