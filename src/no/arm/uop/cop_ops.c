
/*------------------------------------
 * cop_ops.c
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

#include "0ctr.h"
/*================================================================*/
#include "../1imp.h"

#include "uop_dec.h"
#include "uop_opc.h"
#include "uop.h"

#include "../cop/cop.h"
#include "../pfm/pfm.h"
/*================================================================*/
void uop_cop_reg_transfer(struct uop *op);
void uop_cop_reg_transfer(struct uop *op)
{
    struct arm_coprocessor *cur_cop = &cop.coproc[op->coproc.cp_num];

    if (cur_cop->installed) {
        cur_cop->reg_transfer(op->coproc.raw_instruction, cur_cop->data);
    } else {
        /* coprocessor not present, same as an undefined instruction */
        uop_arm_undefined(op);
    }

    pfm_cnt_cop_reg();
}

void uop_cop_double_reg_transfer(struct uop *op);
void uop_cop_double_reg_transfer(struct uop *op)
{
    struct arm_coprocessor *cur_cop = &cop.coproc[op->coproc.cp_num];

    if (cur_cop->installed) {
        cur_cop->double_reg_transfer(op->coproc.raw_instruction, cur_cop->data);
    } else {
        /* coprocessor not present, same as an undefined instruction */
        uop_arm_undefined(op);
    }

    pfm_cnt_cop_reg();
}

void uop_cop_data_processing(struct uop *op);
void uop_cop_data_processing(struct uop *op)
{
    struct arm_coprocessor *cur_cop = &cop.coproc[op->coproc.cp_num];

    if (cur_cop->installed) {
        cur_cop->data_processing(op->coproc.raw_instruction, cur_cop->data);
    } else {
        /* coprocessor not present, same as an undefined instruction */
        uop_arm_undefined(op);
    }

    pfm_cnt_cop_data();
}

void uop_cop_load_store(struct uop *op);
void uop_cop_load_store(struct uop *op)
{
    struct arm_coprocessor *cur_cop = &cop.coproc[op->coproc.cp_num];

    if (cur_cop->installed) {
        cur_cop->load_store(op->coproc.raw_instruction, cur_cop->data);
    } else {
        /* coprocessor not present, same as an undefined instruction */
        uop_arm_undefined(op);
    }

    pfm_cnt_cop_ld_st();
}

/*================================================================*/
/* end of cop_ops.c */
