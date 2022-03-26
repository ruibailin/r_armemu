
/*------------------------------------
 * cpu_s_r.h
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


#ifndef NO_ARM_CPU_CPU_S_R_H_
#define NO_ARM_CPU_CPU_S_R_H_
/*================================================================*/

/* shift and rotate helpers */
#define LSL(val, shift) \
    (((shift) >= 32) ? 0 : ((val) << (shift)))
#define LSR(val, shift) \
    (((shift) >= 32) ? 0 : ((val) >> (shift)))
#define ASR_SIMPLE(val, shift) \
    (((int)(val)) >> (shift))
#define ASR(val, shift) \
    (((shift) >= 32) ? (BIT(val, 31) ? (int)-1 : 0) : (((int)(val)) >> (shift)))
#define ROR(val, shift) \
    (((val) >> (shift)) | ((val) << (32 - (shift))))

/*================================================================*/
#endif 
/* end of cpu_s_r.h */
