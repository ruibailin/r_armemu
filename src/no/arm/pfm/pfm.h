
/*------------------------------------
 * pfm.h
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

#include "../0ctr.h"
#ifndef NO_ARM_PFM_PFM_H_
#define NO_ARM_PFM_PFM_H_
/*================================================================*/
#include "../1imp.h"
#include "pfm_cpu.h"
#include "../uop/uop.h"

/*================================================================*/
//To be compatible to old version
extern int get_cycle_count(void);
extern int get_instruction_count(void);

#if COUNT_MMU_OPS
#define mmu_inc_perf_counter(x) inc_perf_counter(x)
#else
#define mmu_inc_perf_counter(x)
#endif

/*================================================================*/
extern void pfm_cnt_ins(void);
extern void pfm_cnt_exp(void);
extern void pfm_cnt_ins_dec(void);
/*================================================================*/
extern void pfm_cnt_mmu_ops(enum perf_counter_type counter);
extern void pfm_mmu_read();
extern void pfm_mmu_write();
extern void pfm_mmu_ins();
extern void pfm_mmu_fast();
extern void pfm_mmu_slow();
extern void pfm_mmu_tran();
/*================================================================*/
extern void pfm_cyc_inc(void);
extern int pfm_get_cyc_count(void);
extern int pfm_get_ins_count(void);
/*================================================================*/
extern void pfm_cnt_op_skip(struct uop *op);
extern void pfm_cnt_op_nop(struct uop *op);
/*================================================================*/
extern void pfm_cnt_op_ld(struct uop *op);
extern void pfm_cyc_op_ld_im_dword(struct uop *op);
extern void pfm_cyc_op_ld_im_word(struct uop *op);
extern void pfm_cyc_op_ld_im_byte(struct uop *op);
extern void pfm_cyc_op_ld_im_offset(struct uop *op);
extern void pfm_cyc_op_ld_reg_offset(struct uop *op);
extern void pfm_cyc_op_ld_multi(struct uop *op);
extern void pfm_cyc_op_ld_multi_s(struct uop *op);
/*================================================================*/
extern void pfm_cnt_op_st(struct uop *op);
extern void pfm_cyc_op_st_im_offset(struct uop *op);
extern void pfm_cyc_op_st_reg_offset(struct uop *op);
extern void pfm_cyc_op_st_multi(struct uop *op);
extern void pfm_cyc_op_st_multi_s(struct uop *op);
/*================================================================*/
extern void pfm_cnt_op_data(struct uop *op);
extern void pfm_cnt_op_arith_imm(struct uop *op);
extern void pfm_cnt_op_arith_reg(struct uop *op);
extern void pfm_cnt_op_arith_imm_shift(struct uop *op);
extern void pfm_cyc_op_arith_reg_shift(struct uop *op);
extern void pfm_cnt_op_arith_reg_shift(struct uop *op);
/*================================================================*/
extern void pfm_cnt_op_mul(struct uop *op);
extern void pfm_cyc_op_mul(struct uop *op);
extern void pfm_cyc_op_mul_long(struct uop *op);
/*================================================================*/
extern void pfm_cnt_op_b(struct uop *op);
extern void pfm_cyc_op_b(struct uop *op);
/*================================================================*/
extern void pfm_cnt_op_misc(struct uop *op);
extern void pfm_cyc_op_swap(struct uop *op);
extern void pfm_cyc_op_und(struct uop *op);
extern void pfm_cyc_op_swi(struct uop *op);
extern void pfm_cyc_op_bkpt(struct uop *op);
/*================================================================*/
extern void pfm_cnt_op_move(struct uop *op);
extern void pfm_cyc_op_mov_to_sr_imm(struct uop *op);
extern void pfm_cyc_op_mov_to_sr_reg(struct uop *op);
extern void pfm_cyc_op_mov_from_sr(struct uop *op);
/*================================================================*/
extern void pfm_cnt_cop_reg(void);
extern void pfm_cnt_cop_data(void);
extern void pfm_cnt_cop_ld_st(void);
/*================================================================*/
extern void pfm_cnt_uops(struct uop *op);

/*================================================================*/
extern word pfm_speed_timer(word interval);
extern word speedtimer(word interval, void *param);
/*================================================================*/
#endif
/* end of pfm.h */
