
/*------------------------------------
 * cdt_cpu.c
 * Create:  2021-02-14
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
/*================================================================*/
#include "../1imp.h"

#include "cdt_cpu.h"

#include "../cpu/cpu.h"

struct cdt_struct cdt;
/*================================================================*/
bool cdt_check_table(byte condition);
bool cdt_check_table(byte condition)
{
	unsigned short tt;
    // this happens far more often than not
    if (likely(condition == COND_AL))
        return TRUE;
    // check the instructions condition mask against precomputed values of cpsr
    tt = (1 << (condition));
    tt &= cdt.condition_table[cpu.cpsr >> COND_SHIFT];
    if(tt==0)
    	return FALSE;
    return TRUE;
}
/*================================================================*/
int cdt_build_table(void);
int cdt_build_table(void)
{
    int i, j;
    reg_t cpsr;

    for (i=0; i<16; i++) {
        cdt.condition_table[i] = 0;
        for (j=0; j<16; j++) {
            int bit;

            // check the condition field and skip the instruction if necessary
            bit = 0;
            cpsr = i << COND_SHIFT;
            switch (j) {
                case COND_EQ: // Zero set
                    if (cpsr & PSR_CC_ZERO)
                        bit = 1;
                    break;
                case COND_NE: // Zero clear
                    if ((cpsr & PSR_CC_ZERO) == 0)
                        bit = 1;
                    break;
                case COND_CS: // Carry set
                    if (cpsr & PSR_CC_CARRY)
                        bit = 1;
                    break;
                case COND_CC: // Carry clear
                    if ((cpsr & PSR_CC_CARRY) == 0)
                        bit = 1;
                    break;
                case COND_MI: // Negative set
                    if (cpsr & PSR_CC_NEG)
                        bit = 1;
                    break;
                case COND_PL : // Negative clear
                    if ((cpsr & PSR_CC_NEG) == 0)
                        bit = 1;
                    break;
                case COND_VS: // Overflow set
                    if (cpsr & PSR_CC_OVL)
                        bit = 1;
                    break;
                case COND_VC: // Overflow clear
                    if ((cpsr & PSR_CC_OVL) == 0)
                        bit = 1;
                    break;
                case COND_HI: // Carry set and Zero clear
                    if ((cpsr & (PSR_CC_CARRY|PSR_CC_ZERO)) == PSR_CC_CARRY)
                        bit = 1;
                    break;
                case COND_LS: // Carry clear or Zero set
                    if (((cpsr & PSR_CC_CARRY) == 0) || (cpsr & PSR_CC_ZERO))
                        bit = 1;
                    break;
                case COND_GE: { // Negative set and Overflow set, or Negative clear and Overflow clear (N==V)
                    reg_t val = cpsr & (PSR_CC_NEG|PSR_CC_OVL);
                    if (val == 0 || val == (PSR_CC_NEG|PSR_CC_OVL))
                        bit = 1;
                    break;
                }
                case COND_LT: { // Negative set and Overflow clear, or Negative clear and Overflow set (N!=V)
                    reg_t val = cpsr & (PSR_CC_NEG|PSR_CC_OVL);
                    if (val == PSR_CC_NEG || val == PSR_CC_OVL)
                        bit = 1;
                    break;
                }
                case COND_GT: { // Zero clear, and either Negative set or Overflow set, or Negative clear and Overflow clear (Z==0 and N==V)
                    if ((cpsr & PSR_CC_ZERO) == 0) {
                        reg_t val = cpsr & (PSR_CC_NEG|PSR_CC_OVL);
                        if (val == 0 || val == (PSR_CC_NEG|PSR_CC_OVL))
                            bit = 1;
                    }
                    break;
                }
                case COND_LE: { // Zero set, or Negative set and Overflow clear, or Negative clear and Overflow set (Z==1 or N!=V)
                    if (cpsr & PSR_CC_ZERO)
                        bit = 1;

                    reg_t val = cpsr & (PSR_CC_NEG|PSR_CC_OVL);
                    if (val == PSR_CC_NEG || val == PSR_CC_OVL)
                        bit = 1;
                    break;
                }
                case COND_SPECIAL:
                case COND_AL:
                    bit = 1;
                    break;
            }


            os_printf("i %d j %d bit %d\n", i, j, bit);

            if (bit)
                cdt.condition_table[i] |= (1 << j);
        }
      os_printf("condition_table[%d] = 0x%x\n", i, cdt.condition_table[i]);
    }
    return 0;
}

/*================================================================*/
bool check_condition(byte condition);
bool check_condition(byte condition)
{
    // this happens far more often than not
    if (likely(condition == COND_AL))
        return TRUE;
    // check the instructions condition mask against precomputed values of cpsr
    return cdt.condition_table[cpu.cpsr >> COND_SHIFT] & (1 << (condition));
}

int build_condition_table(void);
int build_condition_table(void)
{
	return cdt_build_table();

}
/*================================================================*/
/* end of cdt_cpu.c */
