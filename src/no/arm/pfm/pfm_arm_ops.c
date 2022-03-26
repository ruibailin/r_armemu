
/*------------------------------------
 * pfm_cpu.c
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

#include "../0ctr.h"
/*================================================================*/
#include "pfm_cpu.h"

#include "../cpu/cpu.h"
#include "../isa/isa.h"
#include "../uop/uop.h"
/*================================================================*/
void pfm_cnt_op_skip(struct uop *op);
void pfm_cnt_op_skip(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_SKIPPED_CONDITION);
#endif
}

/*================================================================*/
void pfm_cnt_op_nop(struct uop *op);
void pfm_cnt_op_nop(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_NOP);
#endif
}

/*================================================================*/
void pfm_cnt_op_ld(struct uop *op);
void pfm_cnt_op_ld(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_LOAD);
#endif
}

void pfm_cyc_op_ld_im_dword(struct uop *op);
void pfm_cyc_op_ld_im_dword(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
    if (op->load_immediate.target_reg == PC)
        add_to_perf_counter(CYCLE_COUNT, 4); // on all cores pc loads are 4 cycles
    else if (get_core() == ARM7)
        add_to_perf_counter(CYCLE_COUNT, 2); // on arm7 all other loads are 3
#endif
}

void pfm_cyc_op_ld_im_word(struct uop *op);
void pfm_cyc_op_ld_im_word(struct uop *op)
{
#if COUNT_CYCLES
	// cycle count
    if (op->load_immediate.target_reg == PC)
        add_to_perf_counter(CYCLE_COUNT, 4); // on all cores pc loads are 4 cycles
    else if (get_core() == ARM7)
        add_to_perf_counter(CYCLE_COUNT, 2); // on arm7 all other loads are 3
    else if (get_core() >= ARM9)
        add_to_perf_counter(CYCLE_COUNT, 1); // byte and halfword loads are one cycle slower
#endif
}

void pfm_cyc_op_ld_im_byte(struct uop *op);
void pfm_cyc_op_ld_im_byte(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
    if (op->load_immediate.target_reg == PC)
        add_to_perf_counter(CYCLE_COUNT, 4); // on all cores pc loads are 4 cycles
    else if (get_core() == ARM7)
        add_to_perf_counter(CYCLE_COUNT, 2); // on arm7 all other loads are 3
    else if (get_core() >= ARM9)
        add_to_perf_counter(CYCLE_COUNT, 1); // byte and halfword loads are one cycle slower
#endif
}

void pfm_cyc_op_ld_im_offset(struct uop *op);
void pfm_cyc_op_ld_im_offset(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
    if (op->load_store_immediate_offset.target_reg == PC)
        add_to_perf_counter(CYCLE_COUNT, 4); // on all cores pc loads are 4 cycles
    else if (get_core() == ARM7)
        add_to_perf_counter(CYCLE_COUNT, 2); // on arm7 all other loads are 3
    else if (get_core() >= ARM9 && (op->flags & UOPLSFLAGS_SIZE_MASK) != UOPLSFLAGS_SIZE_WORD)
        add_to_perf_counter(CYCLE_COUNT, 1); // byte, halfword, and dword loads are one cycle slower
#endif
}

void pfm_cyc_op_ld_reg_offset(struct uop *op);
void pfm_cyc_op_ld_reg_offset(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
    if (op->load_store_scaled_reg_offset.target_reg == PC)
        add_to_perf_counter(CYCLE_COUNT, 4); // on all cores pc loads are 4 cycles
    else if (get_core() == ARM7)
        add_to_perf_counter(CYCLE_COUNT, 2); // on arm7 all other loads are 3
    else if (get_core() >= ARM9 && (op->flags & UOPLSFLAGS_SIZE_MASK) != UOPLSFLAGS_SIZE_WORD)
        add_to_perf_counter(CYCLE_COUNT, 1); // byte, halfword, and dword loads are one cycle slower
    if (get_core() == ARM9e)
        add_to_perf_counter(CYCLE_COUNT, 1); // scaled register loads are 1 cycle slower on this core
#endif
}

void pfm_cyc_op_ld_multi(struct uop *op);
void pfm_cyc_op_ld_multi(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
    if (get_core() == ARM7) {
        add_to_perf_counter(CYCLE_COUNT, op->load_store_multiple.reg_count + 1);
        if (op->load_store_multiple.reg_bitmap & 0x8000) // loaded into PC
            add_to_perf_counter(CYCLE_COUNT, 2);
    } else { /* if(get_core() >= ARM9) */
        add_to_perf_counter(CYCLE_COUNT, (op->load_store_multiple.reg_count > 1) ? (op->load_store_multiple.reg_count - 1) : 1);
        if (op->load_store_multiple.reg_bitmap & 0x8000) {
            add_to_perf_counter(CYCLE_COUNT, 4);
            if (get_core() == ARM9e && op->load_store_multiple.reg_count == 0)
                add_to_perf_counter(CYCLE_COUNT, -1); // ldm of just pc is one cycle faster on ARM9e
        }
    }
#endif
}

void pfm_cyc_op_ld_multi_s(struct uop *op);
void pfm_cyc_op_ld_multi_s(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
    if (get_core() == ARM7) {
        add_to_perf_counter(CYCLE_COUNT, op->load_store_multiple.reg_count + 1);
        if (op->load_store_multiple.reg_bitmap & 0x8000) // loaded into PC
            add_to_perf_counter(CYCLE_COUNT, 2);
    } else { /* if(get_core() >= ARM9) */
        add_to_perf_counter(CYCLE_COUNT, (op->load_store_multiple.reg_count > 1) ? (op->load_store_multiple.reg_count - 1) : 1);
        if (op->load_store_multiple.reg_bitmap & 0x8000) {
            add_to_perf_counter(CYCLE_COUNT, 4);
            if (get_core() == ARM9e && op->load_store_multiple.reg_count == 0)
                add_to_perf_counter(CYCLE_COUNT, -1); // ldm of just pc is one cycle faster on ARM9e
        }
    }
#endif
}

/*================================================================*/
void pfm_cnt_op_st(struct uop *op);
void pfm_cnt_op_st(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_STORE);
#endif
}

void pfm_cyc_op_st_im_offset(struct uop *op);
void pfm_cyc_op_st_im_offset(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count (arm9 is 1 cycle, arm7 is 2)
    if (get_core() == ARM7) {
        add_to_perf_counter(CYCLE_COUNT, 1);
    }
    // strd is one cycle slower
    if ((op->flags & UOPLSFLAGS_SIZE_MASK) == UOPLSFLAGS_SIZE_DWORD) {
        add_to_perf_counter(CYCLE_COUNT, 1);
    }
#endif
}

void pfm_cyc_op_st_reg_offset(struct uop *op);
void pfm_cyc_op_st_reg_offset(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count (arm9 is 1 cycle, arm7 is 2)
    if (get_core() == ARM7) {
        add_to_perf_counter(CYCLE_COUNT, 1);
    } else if (get_core() == ARM9e) {
        add_to_perf_counter(CYCLE_COUNT, 1); // XXX not precisely correct, since a zero scale is no extra work
    }
    // strd is one cycle slower
    if ((op->flags & UOPLSFLAGS_SIZE_MASK) == UOPLSFLAGS_SIZE_DWORD) {
        add_to_perf_counter(CYCLE_COUNT, 1);
    }
#endif
}

void pfm_cyc_op_st_multi(struct uop *op);
void pfm_cyc_op_st_multi(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
    if (get_core() == ARM7) {
        add_to_perf_counter(CYCLE_COUNT, (op->load_store_multiple.reg_count - 1) + 1);
    } else { /* if(get_core() >= ARM9) */
        add_to_perf_counter(CYCLE_COUNT, (op->load_store_multiple.reg_count > 1) ? (op->load_store_multiple.reg_count - 1) : 1);
    }
#endif
}

void pfm_cyc_op_st_multi_s(struct uop *op);
void pfm_cyc_op_st_multi_s(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
    if (get_core() == ARM7) {
        add_to_perf_counter(CYCLE_COUNT, (op->load_store_multiple.reg_count - 1) + 1);
    } else { /* if(get_core() >= ARM9) */
        add_to_perf_counter(CYCLE_COUNT, (op->load_store_multiple.reg_count > 1) ? (op->load_store_multiple.reg_count - 1) : 1);
    }
#endif
}

/*================================================================*/
void pfm_cnt_op_data(struct uop *op);
void pfm_cnt_op_data(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_DATA_PROC);
#endif
}

/*================================================================*/
void pfm_cnt_op_mul(struct uop *op);
void pfm_cnt_op_mul(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_MUL);
#endif
}

void pfm_cyc_op_mul(struct uop *op);
void pfm_cyc_op_mul(struct uop *op)
{
#if COUNT_CYCLES
    // cycle count
	 dword temp_word = get_reg(op->mul.source_reg);
    if (get_core() <= ARM9) {
        int signed_word = temp_word;

        if ((signed_word >> 8) == 0 || (signed_word >> 8) == -1)
            add_to_perf_counter(CYCLE_COUNT, 1);
        else if ((signed_word >> 16) == 0 || (signed_word >> 16) == -1)
            add_to_perf_counter(CYCLE_COUNT, 2);
        else if ((signed_word >> 24) == 0 || (signed_word >> 24) == -1)
            add_to_perf_counter(CYCLE_COUNT, 3);
    } else { /* if(get_core() == ARM9e) */
        /* ARM9e core can do the multiply in 2 cycles, with an interlock */
        add_to_perf_counter(CYCLE_COUNT, 1);
        // XXX schedule interlock here
    }
#endif
}

void pfm_cyc_op_mul_long(struct uop *op);
void pfm_cyc_op_mul_long(struct uop *op)
{
#if COUNT_CYCLES
    dword temp_word = get_reg(op->mull.source_reg);
    // cycle count
    if (get_core() <= ARM9) {
        if ((temp_word >> 8) == 0)
            add_to_perf_counter(CYCLE_COUNT, 2);
        else if ((temp_word >> 16) == 0)
            add_to_perf_counter(CYCLE_COUNT, 3);
        else if ((temp_word >> 24) == 0)
            add_to_perf_counter(CYCLE_COUNT, 4);
    } else { /* if(get_core() == ARM9e) */
        /* ARM9e core can do the multiply in 3 cycles, with an interlock */
        add_to_perf_counter(CYCLE_COUNT, 2);
        // XXX schedule interlock here
    }
#endif
}

/*================================================================*/
void pfm_cnt_op_b(struct uop *op);
void pfm_cnt_op_b(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_BRANCH);
#endif
}

void pfm_cyc_op_b(struct uop *op);
void pfm_cyc_op_b(struct uop *op)
{
#if COUNT_CYCLES
    // all branch instructions take 3 cycles on all cores
    add_to_perf_counter(CYCLE_COUNT, 2);
#endif
}

/*================================================================*/
void pfm_cnt_op_misc(struct uop *op);
void pfm_cnt_op_misc(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_MISC);
#endif
}

void pfm_cyc_op_und(struct uop *op);
void pfm_cyc_op_und(struct uop *op)
{
#if COUNT_CYCLES
    if (get_core() == ARM7)
        add_to_perf_counter(CYCLE_COUNT, 3);
    else /* if(get_core() >= ARM9) */
        add_to_perf_counter(CYCLE_COUNT, 2);
#endif
}

void pfm_cyc_op_swap(struct uop *op);
void pfm_cyc_op_swap(struct uop *op)
{
#if COUNT_CYCLES
    // XXX cycle count
    if (get_core() == ARM7) {
        add_to_perf_counter(CYCLE_COUNT, 3);
    } else { /* if(get_core() >= ARM9) */
        add_to_perf_counter(CYCLE_COUNT, 1);
        // XXX schedule interlock
    }
#endif
}

void pfm_cyc_op_swi(struct uop *op);
void pfm_cyc_op_swi(struct uop *op)
{
    // always takes 3 cycles
#if COUNT_CYCLES
    add_to_perf_counter(CYCLE_COUNT, 2);
#endif
}

void pfm_cyc_op_bkpt(struct uop *op);
void pfm_cyc_op_bkpt(struct uop *op)
{
    // always takes 3 cycles
#if COUNT_CYCLES
    add_to_perf_counter(CYCLE_COUNT, 2);
#endif
}

/*================================================================*/
void pfm_cnt_op_move(struct uop *op);
void pfm_cnt_op_move(struct uop *op)
{
#if COUNT_ARM_OPS
    inc_perf_counter(OP_MISC);
#endif
}

void pfm_cyc_op_mov_to_sr_imm(struct uop *op);
void pfm_cyc_op_mov_to_sr_imm(struct uop *op)
{
#if COUNT_CYCLES
	   word field_mask = op->move_to_sr_imm.field_mask;
	    if (!arm_in_priviledged())
	        field_mask &= 0xff000000;
        // cycle count
        if (field_mask & 0x00ffffff)
            add_to_perf_counter(CYCLE_COUNT, 2); // we updated something other than the status flags
#endif
}

void pfm_cyc_op_mov_to_sr_reg(struct uop *op);
void pfm_cyc_op_mov_to_sr_reg(struct uop *op)
{
#if COUNT_CYCLES
	   word field_mask = op->move_to_sr_imm.field_mask;
	    if (!arm_in_priviledged())
	        field_mask &= 0xff000000;
        // cycle count
        if (field_mask & 0x00ffffff)
            add_to_perf_counter(CYCLE_COUNT, 2); // we updated something other than the status flags
#endif
}

void pfm_cyc_op_mov_from_sr(struct uop *op);
void pfm_cyc_op_mov_from_sr(struct uop *op)
{
#if COUNT_CYCLES
    // 2 cycles on arm9+
    if (get_core() >= ARM9)
        add_to_perf_counter(CYCLE_COUNT, 1);
#endif
}

/*================================================================*/
/* end of pfm_cpu.c */
