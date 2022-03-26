
/*------------------------------------
 * bus_addr.h
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

#ifndef armaddr_t
#define armaddr_t  u32
#endif

#ifndef addr_t
#define addr_t  u32
#endif

#ifndef reg_t
#define reg_t  u32
#endif

#define REG64(x) ((volatile u64 *)(x))
#define REG(x) ((volatile u32 *)(x))
#define REG_H(x) ((volatile u16 *)(x))
#define REG_B(x) ((volatile u8 *)(x))

#define  BUS_BAR_ADD_LINE	22			//the device connected to bus bar at least uses 22 address line and 4M space
/*================================================================*/
#endif
/* end of bus_addr.h */
