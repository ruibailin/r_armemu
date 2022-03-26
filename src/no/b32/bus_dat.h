
/*------------------------------------
 * bus_dat.h
 * Create:  2021-02-03
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
#ifndef NO_B32_BUS_DAT_H_
#define NO_B32_BUS_DAT_H_
/*================================================================*/
#include "../1imp.h"

// define a macro that conditionally swaps
#define SWAP_DWORD(x) \
    (((x) >> 24) | (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) | ((x) << 24))
#define SWAP_WORD(x) \
    (((x) >> 8) | ((x) << 8))

#if CPU_NEEDS_SWAP
#define SWAPIT_DWORD(x) SWAP_DWORD(x)
#define SWAPIT_WORD(x) SWAP_WORD(x)
#else
#define SWAPIT_DWORD(x) (x)
#define SWAPIT_WORD(x) (x)
#endif

// some memory access macros

#define READ_MEM_DWORD(ptr)     	SWAPIT_DWORD(*(dword *)(ptr))
#define READ_MEM_WORD(ptr)  		SWAPIT_WORD(*(word *)(ptr))
#define READ_MEM_BYTE(ptr)      	(*(byte *)(ptr))

#define WRITE_MEM_DWORD(ptr, data)   	(*(dword *)(ptr) = SWAPIT_DWORD(data))
#define WRITE_MEM_WORD(ptr, data)   	(*(word *)(ptr) = SWAPIT_WORD(data))
#define WRITE_MEM_BYTE(ptr, data)   	(*(byte *)(ptr) = (data))

//actually only can be used in host
#define HOST_READ_MEM_DWORD(ptr)     	SWAPIT_DWORD(*(dword *)(ptr))
#define HOST_READ_MEM_WORD(ptr)  		SWAPIT_WORD(*(word *)(ptr))
#define HOST_READ_MEM_BYTE(ptr)      	(*(byte *)(ptr))

#define HOST_WRITE_MEM_DWORD(ptr, data)   	(*(dword *)(ptr) = SWAPIT_DWORD(data))
#define HOST_WRITE_MEM_WORD(ptr, data)   	(*(word *)(ptr) = SWAPIT_WORD(data))
#define HOST_WRITE_MEM_BYTE(ptr, data)   	(*(byte *)(ptr) = (data))
/*================================================================*/
#endif

/* end of bus_dat.h */
