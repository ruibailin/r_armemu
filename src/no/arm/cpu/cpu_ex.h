
/*------------------------------------
 * cpu_ex.h
 * Create:  2021-02-10
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */


#ifndef NO_ARM_CPU_CPU_EX_H_
#define NO_ARM_CPU_CPU_EX_H_
/*================================================================*/
/* pending exception bits */
#define EX_RESET      0x01
#define EX_UNDEFINED  0x02
#define EX_SWI        0x04
#define EX_PREFETCH   0x08
#define EX_DATA_ABT   0x10
#define EX_FIQ        0x40      /* same bits as the cpsr mask bits */
#define EX_IRQ        0x80

/*================================================================*/
#endif 
/* end of cpu_ex.h */
