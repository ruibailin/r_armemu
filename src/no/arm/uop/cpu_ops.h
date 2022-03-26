
/*------------------------------------
 * cpu_ops.h
 * Create:  2021-02-13
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */

#ifndef NO_ARM_UOP_CPU_OPS_H_
#define NO_ARM_UOP_CPU_OPS_H_
/*================================================================*/
#include "../1imp.h"
#include "uop_dec.h"
/*================================================================*/
extern void uop_decode_me_arm(struct uop *op);
extern void uop_decode_me_thumb(struct uop *op);
/*================================================================*/
extern void uop_b_immediate(struct uop *op);
extern void uop_b_immediate_local(struct uop *op);
extern void uop_b_reg(struct uop *op);
extern void uop_b_reg_offset(struct uop *op);
/*================================================================*/
extern void uop_load_immediate_dword(struct uop *op);
extern void uop_load_immediate_word(struct uop *op);
extern void uop_load_immediate_byte(struct uop *op);
extern void uop_load_immediate_offset(struct uop *op);
extern void uop_load_scaled_reg_offset(struct uop *op);
extern void uop_load_multiple(struct uop *op);
extern void uop_load_multiple_s(struct uop *op);
/*================================================================*/
extern void uop_store_immediate_offset(struct uop *op);
extern void uop_store_scaled_reg_offset(struct uop *op);
extern void uop_store_multiple(struct uop *op);
extern void uop_store_multiple_s(struct uop *op);
/*================================================================*/
extern void uop_data_processing_imm(struct uop *op);
extern void uop_data_processing_reg(struct uop *op);
extern void uop_data_processing_imm_s(struct uop *op);
extern void uop_data_processing_reg_s(struct uop *op);
extern void uop_data_processing_imm_shift(struct uop *op);
extern void uop_data_processing_reg_shift(struct uop *op);
/*================================================================*/
extern void uop_mov_imm(struct uop *op);
extern void uop_mov_imm_nz(struct uop *op);
extern void uop_mov_reg(struct uop *op);
/*================================================================*/
extern void uop_cmp_imm_s(struct uop *op);
extern void uop_cmp_reg_s(struct uop *op);
extern void uop_cmn_reg_s(struct uop *op);
extern void uop_tst_reg_s(struct uop *op);
/*================================================================*/
extern void uop_add_imm(struct uop *op);
extern void uop_add_imm_s(struct uop *op);
extern void uop_add_reg(struct uop *op);
extern void uop_add_reg_s(struct uop *op);
extern void uop_adc_reg_s(struct uop *op);
extern void uop_sub_reg_s(struct uop *op);
extern void uop_sbc_reg_s(struct uop *op);
/*================================================================*/
extern void uop_and_imm(struct uop *op);
extern void uop_orr_imm(struct uop *op);
extern void uop_orr_reg_s(struct uop *op);
/*================================================================*/
extern void uop_lsl_imm(struct uop *op);
extern void uop_lsl_imm_s(struct uop *op);
extern void uop_lsl_reg(struct uop *op);
extern void uop_lsl_reg_s(struct uop *op);
extern void uop_lsr_imm(struct uop *op);
extern void uop_lsr_imm_s(struct uop *op);
extern void uop_lsr_reg(struct uop *op);
extern void uop_lsr_reg_s(struct uop *op);
extern void uop_asr_imm(struct uop *op);
extern void uop_asr_imm_s(struct uop *op);
extern void uop_asr_reg(struct uop *op);
extern void uop_asr_reg_s(struct uop *op);
extern void uop_ror_reg(struct uop *op);
extern void uop_ror_reg_s(struct uop *op);
/*================================================================*/
extern void uop_and_reg_s(struct uop *op);
extern void uop_eor_reg_s(struct uop *op);
/*================================================================*/
extern void uop_bic_reg_s(struct uop *op);
extern void uop_neg_reg_s(struct uop *op);
extern void uop_mvn_reg_s(struct uop *op);
/*================================================================*/
extern void uop_multiply(struct uop *op);
extern void uop_multiply_long(struct uop *op);
/*================================================================*/
extern void uop_swap(struct uop *op);
/*================================================================*/
extern void uop_count_leading_zeros(struct uop *op);
/*================================================================*/
extern void uop_move_to_sr_imm(struct uop *op);
extern void uop_move_to_sr_reg(struct uop *op);
extern void uop_move_from_sr(struct uop *op);
/*================================================================*/
extern void uop_undefined(struct uop *op);
/*================================================================*/
extern void uop_swi(struct uop *op);
extern void uop_bkpt(struct uop *op);
/*================================================================*/
#endif
/* end of cpu_ops.h */
