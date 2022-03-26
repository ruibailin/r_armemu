
/*------------------------------------
 * arm_ops.h
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

#ifndef NO_ARM_UOP_ARM_OPS_H_
#define NO_ARM_UOP_ARM_OPS_H_
/*================================================================*/
#include "uop_opc.h"
#include "uop_dec.h"
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
#endif
/* end of arm_ops.h */
