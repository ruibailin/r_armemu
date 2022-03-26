
/*------------------------------------
 * thumb_ops.h
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

#ifndef NO_ARM_UOP_THUMB_OPS_H_
#define NO_ARM_UOP_THUMB_OPS_H_
/*================================================================*/
#include "uop_opc.h"
#include "uop_dec.h"

/*================================================================*/
/* thumb ops */
extern void uop_tmb_shift_imm(struct uop *op);
extern void uop_tmb_add_sub_3reg(struct uop *op);
extern void uop_tmb_add_sub_large_immediate(struct uop *op);
extern void uop_tmb_data_processing(struct uop *op);
extern void uop_tmb_special_data_processing(struct uop *op);
extern void uop_tmb_bx_reg(struct uop *op);
extern void uop_tmb_literal_load(struct uop *op);
extern void uop_tmb_load_store_register(struct uop *op);
extern void uop_tmb_load_store_immediate_offset(struct uop *op);
extern void uop_tmb_load_store_from_stack(struct uop *op);
extern void uop_tmb_add_to_sp_pc(struct uop *op);
extern void uop_tmb_adjust_sp(struct uop *op);
extern void uop_tmb_push_pop_regs(struct uop *op);
extern void uop_tmb_software_breakpoint(struct uop *op);
extern void uop_tmb_undefined(struct uop *op);
extern void uop_tmb_load_store_multiple(struct uop *op);
extern void uop_tmb_conditional_branch(struct uop *op);
extern void uop_tmb_swi(struct uop *op);
extern void uop_tmb_branch(struct uop *op);
extern void uop_tmb_bl_blx(struct uop *op);
extern void uop_tmb_blx_suffix(struct uop *op);
extern void uop_tmb_bl_suffix(struct uop *op);

/*================================================================*/
//To be compatible to old version
/* thumb ops */
extern void thumb_op_shift_imm(struct uop *op);
extern void thumb_op_add_sub_3reg(struct uop *op);
extern void thumb_op_add_sub_large_immediate(struct uop *op);
extern void thumb_op_data_processing(struct uop *op);
extern void thumb_op_special_data_processing(struct uop *op);
extern void thumb_op_bx_reg(struct uop *op);
extern void thumb_op_literal_load(struct uop *op);
extern void thumb_op_load_store_register(struct uop *op);
extern void thumb_op_load_store_immediate_offset(struct uop *op);
extern void thumb_op_load_store_from_stack(struct uop *op);
extern void thumb_op_add_to_sp_pc(struct uop *op);
extern void thumb_op_adjust_sp(struct uop *op);
extern void thumb_op_push_pop_regs(struct uop *op);
extern void thumb_op_software_breakpoint(struct uop *op);
extern void thumb_op_undefined(struct uop *op);
extern void thumb_op_load_store_multiple(struct uop *op);
extern void thumb_op_conditional_branch(struct uop *op);
extern void thumb_op_swi(struct uop *op);
extern void thumb_op_branch(struct uop *op);
extern void thumb_op_bl_blx(struct uop *op);
extern void thumb_op_blx_suffix(struct uop *op);
extern void thumb_op_bl_suffix(struct uop *op);

/*================================================================*/
#endif 
/* end of thumb_ops.h */
