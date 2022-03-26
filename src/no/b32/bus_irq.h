
/*------------------------------------
 * bus_irq.h
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
#ifndef NO_B32_BUS_IRQ_H_
#define NO_B32_BUS_IRQ_H_
/*================================================================*/
#include "../1imp.h"

extern void bus_raise_irq();			//send raise irq signal to CPU
extern void bus_lower_irq();			//send lower irq signal to CPU

extern void bus_raise_fiq();			//send raise irq signal to float CPU
extern void bus_lower_fiq();			//send lower irq signal to float CPU

/* interrupt map */
#define INT_PIT      0
#define INT_KEYBOARD 1
#define INT_NET      2
/*================================================================*/
#endif
/* end of bus_irq.h */
