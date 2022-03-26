
/*------------------------------------
 * pfm_arith_uops.c
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
/*================================================================*/
#include "pfm_cpu.h"
#include "../uop/uop_dec.h"

/*================================================================*/
void pfm_cnt_op_arith_imm(struct uop *op);
void pfm_cnt_op_arith_imm(struct uop *op)
{
#if COUNT_ARITH_UOPS
    inc_perf_counter(UOP_ARITH_OPCODE + op->data_processing_imm.dp_opcode);
#endif
}

void pfm_cnt_op_arith_reg(struct uop *op);
void pfm_cnt_op_arith_reg(struct uop *op)
{
#if COUNT_ARITH_UOPS
    inc_perf_counter(UOP_ARITH_OPCODE + op->data_processing_reg.dp_opcode);
#endif
}

void pfm_cnt_op_arith_imm_shift(struct uop *op);
void pfm_cnt_op_arith_imm_shift(struct uop *op)
{
#if COUNT_ARITH_UOPS
    inc_perf_counter(UOP_ARITH_OPCODE + op->data_processing_imm_shift.dp_opcode);
#endif
}

void pfm_cyc_op_arith_reg_shift(struct uop *op);
void pfm_cyc_op_arith_reg_shift(struct uop *op)
{
#if COUNT_CYCLES
    /* shifting by a reg value costs an extra cycle */
    add_to_perf_counter(CYCLE_COUNT, 1);
#endif
}

void pfm_cnt_op_arith_reg_shift(struct uop *op);
void pfm_cnt_op_arith_reg_shift(struct uop *op)
{
#if COUNT_ARITH_UOPS
    inc_perf_counter(UOP_ARITH_OPCODE + op->data_processing_reg_shift.dp_opcode);
#endif
}
/*================================================================*/
/* end of pfm_arith_uops.c */
