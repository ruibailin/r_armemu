
/*------------------------------------
 * cpu_sign.h
 * Create:  2021-02-11
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */


#ifndef NO_ARM_CPU_CPU_SIGN_H_
#define NO_ARM_CPU_CPU_SIGN_H_
/*================================================================*/
#include "../1imp.h"

#include "cpu_bits.h"

#define ISNEG(x) BIT((x), 31)
#define ISPOS(x) (!(BIT(x, 31)))

/* 32-bit sign extension */
//#define SIGN_EXTEND(val, topbit) (BIT(val, topbit) ? ((val) | (0xffffffff << (topbit))) : (val))
#define SIGN_EXTEND(val, topbit) (ASR_SIMPLE(LSL(val, 32-(topbit)), 32-(topbit)))

static inline dword do_add(dword a, dword b, int carry_in, int *carry, int *ovl)
{
    dword val;

    if (!IS_64HOST) {
        val = a + b + carry_in;

        *carry = (ISNEG(a & b) ||               // both operands are negative, or
                  (ISNEG(a ^ b) && ISPOS(val))); // exactly one of the operands is negative, and result is positive
    } else {
        /* 64 bit version of the add routine to optimize for the carry flag test */
        qword bigval,bigv2;

        bigval = (qword)a + (qword)b + (qword)carry_in;
        bigv2 = BIT_SHIFT(bigval, 32);
        *carry = (dword)bigv2;
        val = (dword)bigval;
    }

    *ovl = (!(ISNEG(a ^ b))) && (ISNEG(a ^ val));

//  CPU_TRACE(10, "do_add: a 0x%x b 0x%x carry_in 0x%x, carry %d, ovl %d\n",
//      a, b, carry_in, *carry, *ovl);

#if 0
#if __i386__ || __I386__
    asm("setc (%0)" :: "r"(carry));
    asm("seto (%0)" :: "r"(ovl));
#endif
#endif

    return val;
}
/*================================================================*/
#endif 
/* end of cpu_sign.h */
