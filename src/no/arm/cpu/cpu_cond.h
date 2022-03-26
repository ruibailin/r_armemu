
/*------------------------------------
 * cpu_cond.h
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


#ifndef NO_ARM_CPU_CPU_COND_H_
#define NO_ARM_CPU_CPU_COND_H_
/*================================================================*/
// conditions
#define COND_MASK     0xf
#define COND_EQ       0x0
#define COND_NE       0x1
#define COND_CS       0x2
#define COND_CC       0x3
#define COND_MI       0x4
#define COND_PL       0x5
#define COND_VS       0x6
#define COND_VC       0x7
#define COND_HI       0x8
#define COND_LS       0x9
#define COND_GE       0xa
#define COND_LT       0xb
#define COND_GT       0xc
#define COND_LE       0xd
#define COND_AL       0xe
#define COND_SPECIAL  0xf
#define COND_SHIFT    28

/*================================================================*/
#endif 
/* end of cpu_cond.h */
