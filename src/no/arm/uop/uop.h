
/*------------------------------------
 * uop.h
 * Create:  2021-02-12
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */

#ifndef NO_ARM_UOP_UOP_H_
#define NO_ARM_UOP_UOP_H_
/*================================================================*/
#include "uop_opc.h"
#include "uop_dec.h"

/*================================================================*/
//To be compatible to old version
/* ARM ops */
extern void op_branch(struct uop *op);
extern void op_bx(struct uop *op);
extern void op_msr(struct uop *op);
extern void op_mrs(struct uop *op);
extern void op_undefined(struct uop *op);
extern void op_data_processing(struct uop *op);
extern void op_mul(struct uop *op);
extern void op_mull(struct uop *op);
extern void op_swap(struct uop *op);
extern void op_load_store(struct uop *op);
extern void op_load_store_multiple(struct uop *op);
extern void op_misc_load_store(struct uop *op);
extern void op_swi(struct uop *op);
extern void op_bkpt(struct uop *op);
extern void op_clz(struct uop *op);
extern void op_pld(struct uop *op);
extern void op_coproc_reg_transfer(struct uop *op);
extern void op_coproc_double_reg_transfer(struct uop *op);
extern void op_coproc_data_processing(struct uop *op);
extern void op_coproc_load_store(struct uop *op);
extern void op_no_coproc_installed(struct uop *op, int cp_num);
/*================================================================*/
//To be compatible to old version
/* data processing opcode to string */
extern const char *dp_op_to_str(int op);
extern const char *uop_opcode_to_str(int opcode);
extern void thumb_decode_into_uop(struct uop *op);
extern void arm_decode_into_uop(struct uop *op);
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
/* ARM ops */
extern void uop_arm_branch(struct uop *op);
extern void uop_arm_bx(struct uop *op);
extern void uop_arm_msr(struct uop *op);
extern void uop_arm_mrs(struct uop *op);
extern void uop_arm_undefined(struct uop *op);
extern void uop_arm_data_processing(struct uop *op);
extern void uop_arm_mul(struct uop *op);
extern void uop_arm_mull(struct uop *op);
extern void uop_arm_swap(struct uop *op);
extern void uop_arm_load_store(struct uop *op);
extern void uop_arm_load_store_multiple(struct uop *op);
extern void uop_arm_misc_load_store(struct uop *op);
extern void uop_arm_swi(struct uop *op);
extern void uop_arm_bkpt(struct uop *op);
extern void uop_arm_clz(struct uop *op);
extern void uop_arm_pld(struct uop *op);
extern void uop_arm_cop_reg_transfer(struct uop *op);
extern void uop_arm_cop_double_reg_transfer(struct uop *op);
extern void uop_arm_cop_data_processing(struct uop *op);
extern void uop_arm_cop_load_store(struct uop *op);
extern void uop_arm_no_coproc_installed(struct uop *op, int cp_num);
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
/* data processing opcode to string */
extern const char *uop_dpc_to_str(int op);
extern const char *uop_opc_to_str(int opc);
extern void uop_from_tmb_decode(struct uop *op);
extern void uop_from_arm_decode(struct uop *op);
extern void uop_dump_cpu(struct uop *op);
extern struct uop *uop_from_cpu_decode(struct uop *op);
/*================================================================*/
#endif 
/* end of uop.h */
