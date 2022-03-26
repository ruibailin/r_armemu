
/*------------------------------------
 * 0ctr.h
 * Create:  2021-01-31
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */

#ifndef NO_0CTR_H_
#define NO_0CTR_H_
/*================================================================*/
#define ARM_32_BUS				1
#define ARM_64_BUS				0

//configure for performance statics
#define COUNT_CYCLES    		1 // should we try to accurately count cycles
#define COUNT_ARM_OPS   		1
#define COUNT_UOPS      		1
#define COUNT_ARITH_UOPS 		1
#define COUNT_MMU_OPS   		1

// lets you change the trace levels at-will, at the expense of some runtime speed
#define DYNAMIC_TRACE_LEVELS 	1

// should we run a thread that dumps stats once a second
#define DUMP_STATS      		1

#define	ARM_HAS_MMU				1

/*================================================================*/
#endif
/* end of 0ctr.h */
