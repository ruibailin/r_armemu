
/*------------------------------------
 * sys_bug.c
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


struct sys_debug {
    addr_t memory_dump_addr;
    unsigned int memory_dump_len;
};

static struct sys_debug debug;

/*================================================================*/
static void dump_memory_byte(addr_t address, unsigned int len);
static void dump_memory_byte(addr_t address, unsigned int len)
{
    int i = 0;
    while (len > 0) {
        byte data = bus_read_u8(address);

        if ((i % 16 == 0)) {
            if (i != 0)
                os_printf("\n");
            os_printf("0x%08x: ", address);
        }

        os_printf("%02x ", data);

        i++;
        len--;
        address++;
    }
    os_printf("\n");
}

static void dump_memory_word(addr_t address, unsigned int len);
static void dump_memory_word(addr_t address, unsigned int len)
{
    int i = 0;

    while (len > 0) {
        word data = bus_read_u16(address);

        if ((i % 16 == 0)) {
            if (i != 0)
                os_printf("\n");
            os_printf("0x%08x: ", address);
        }

        os_printf("%04x ", data);

        i++;
        len--;
        address += 2;
    }
    os_printf("\n");
}

static void dump_memory_dword(addr_t address, unsigned int len);
static void dump_memory_dword(addr_t address, unsigned int len)
{
    int i = 0;

    while (len > 0) {
        dword data = bus_read_u32(address);

        if ((i % 16 == 0)) {
            if (i != 0)
                os_printf("\n");
            os_printf("0x%08x: ", address);
        }

        os_printf("%08x ", data);

        i++;
        len--;
        address += 4;
    }
    os_printf("\n");
}


/*================================================================*/
#define DEBUG_STDOUT (DEBUG_REGS_BASE + 0) /* writes to this register are sent through to stdout */
#define DEBUG_STDIN  (DEBUG_REGS_BASE + 0) /* reads from this register return the contents of stdin
                                            * or -1 if no data is pending */
#define DEBUG_REGDUMP (DEBUG_REGS_BASE + 4) /* writes to this register cause the emulator to dump registers */
#define DEBUG_HALT    (DEBUG_REGS_BASE + 8) /* writes to this register will halt the emulator */

#define DEBUG_MEMDUMPADDR (DEBUG_REGS_BASE + 12)      /* set the base address of memory to dump */
#define DEBUG_MEMDUMPLEN  (DEBUG_REGS_BASE + 16)      /* set the length of memory to dump */
#define DEBUG_MEMDUMP_BYTE  (DEBUG_REGS_BASE + 20)    /* trigger a memory dump in byte format */
#define DEBUG_MEMDUMP_WORD (DEBUG_REGS_BASE + 24) /* trigger a memory dump in halfword format */
#define DEBUG_MEMDUMP_DWORD (DEBUG_REGS_BASE + 28)     /* trigger a memory dump in word format */

/* lets you set the trace level of the various subsystems from within the emulator */
/* only works on emulator builds that support dynamic trace levels */
#define DEBUG_SET_TRACELEVEL_CPU (DEBUG_REGS_BASE + 32)
#define DEBUG_SET_TRACELEVEL_UOP (DEBUG_REGS_BASE + 36)
#define DEBUG_SET_TRACELEVEL_SYS (DEBUG_REGS_BASE + 40)
#define DEBUG_SET_TRACELEVEL_MMU (DEBUG_REGS_BASE + 44)

#define DEBUG_CYCLE_COUNT (DEBUG_REGS_BASE + 48)
#define DEBUG_INS_COUNT (DEBUG_REGS_BASE + 52)

#include "../arm/pfm/pfm.h"
#include "../arm/cpu/cpu.h"
static dword debug_get_put(addr_t address, dword data, int size, int put);
static dword debug_get_put(addr_t address, dword data, int size, int put)
{
    char x;

    if (put) {
        switch (address) {
            case DEBUG_STDOUT:
                x = data;
                os_write(1, &x, 1);
                return 0;
            case DEBUG_REGDUMP:
               dump_registers();
                return 0;
            case DEBUG_HALT:
                if (data == 1)
                    os_panic("debug halt\n");
                return 0;
            case DEBUG_MEMDUMPADDR:
                debug.memory_dump_addr = data;
                return 0;
            case DEBUG_MEMDUMPLEN:
                debug.memory_dump_len = data;
                return 0;
            case DEBUG_MEMDUMP_BYTE:
                dump_memory_byte(debug.memory_dump_addr, debug.memory_dump_len);
                return 0;
            case DEBUG_MEMDUMP_WORD:
                dump_memory_word(debug.memory_dump_addr, debug.memory_dump_len);
                return 0;
            case DEBUG_MEMDUMP_DWORD:
                dump_memory_dword(debug.memory_dump_addr, debug.memory_dump_len);
                return 0;
#if DYNAMIC_TRACE_LEVELS
            case DEBUG_SET_TRACELEVEL_CPU:
                TRACE_CPU_LEVEL = data;
                return 0;
            case DEBUG_SET_TRACELEVEL_UOP:
                TRACE_UOP_LEVEL = data;
                return 0;
            case DEBUG_SET_TRACELEVEL_SYS:
                TRACE_SYS_LEVEL = data;
                return 0;
            case DEBUG_SET_TRACELEVEL_MMU:
                TRACE_MMU_LEVEL = data;
                return 0;
#endif
            default:
                return 0;
        }
    } else {
        switch (address) {
            case DEBUG_STDIN:
                if (os_read(0, &x, 1) == 1) {
                    return x;
                } else {
                    return -1;
                }
#if COUNT_CYCLES
            case DEBUG_CYCLE_COUNT:
               return get_cycle_count();
#endif
            case DEBUG_INS_COUNT:
                return get_instruction_count();
            default:
                return 0;
        }
    }
}


/*================================================================*/
void sys_ini_bug(void);
void sys_ini_bug(void)
{
	int host_mem_len;
	void *host_mem_addr;
    os_memclr(&debug, sizeof(debug));

//    host_mem_len = DEBUG_INS_COUNT+4-DEBUG_REGS_BASE;
//    host_mem_addr=os_malloc(host_mem_len);
	host_mem_len = sizeof(debug);
	host_mem_addr = &debug;
	bus_set_func(DEBUG_REGS_BASE,debug_get_put,NULL);
	bus_set_host_base_addr(DEBUG_REGS_BASE,host_mem_addr);
	bus_bat_set_func(DEBUG_REGS_BASE, host_mem_len);

}

void sys_del_bug(void);
void sys_del_bug(void)
{
	void *host_mem_addr;
	host_mem_addr = bus_get_mem_ptr(DEBUG_REGS_BASE);
	os_free(host_mem_addr);
}
/*================================================================*/

/* end of sys_bug.c */
