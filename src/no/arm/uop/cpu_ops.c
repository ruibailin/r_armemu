
/*------------------------------------
 * cpu_ops.c
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

#include "../cpu/cpu.h"
#include "../cdp/cdp.h"
#include "../mmu/mmu.h"
#include "../pfm/pfm.h"
/*================================================================*/
#define ASSERT_VALID_REG(x) ASSERT((x) < 16);

#define DATA_PROCESSING_OP_TABLE(opcode, result, a, b, arith_op, Rd_writeback, carry, ovl) \
    switch(opcode) { \
        case AOP_AND: /* AND */ \
            result = a & b; \
            break; \
        case AOP_EOR: /* EOR */ \
            result = a ^ b; \
            break; \
        case AOP_SUB: /* SUB */ \
            result = do_add(a, ~b, 1, &carry, &ovl); \
            arith_op = 1; \
            break; \
        case AOP_RSB: /* RSB */ \
            result = do_add(b, ~a, 1, &carry, &ovl); \
            arith_op = 1; \
            break; \
        case AOP_ADD: /* ADD */ \
            result = do_add(a, b, 0, &carry, &ovl); \
            arith_op = 1; \
            break; \
        case AOP_ADC: /* ADC */ \
            result = do_add(a, b, get_condition(PSR_CC_CARRY) ? 1 : 0, &carry, &ovl); \
            arith_op = 1; \
            break; \
        case AOP_SBC: /* SBC */ \
            result = do_add(a, ~b, get_condition(PSR_CC_CARRY) ? 1 : 0, &carry, &ovl); \
            arith_op = 1; \
            break; \
        case AOP_RSC: /* RSC */ \
            result = do_add(b, ~a, get_condition(PSR_CC_CARRY) ? 1 : 0, &carry, &ovl); \
            arith_op = 1; \
            break; \
        case AOP_TST: /* TST */ \
            result = a & b; \
            Rd_writeback = 0; \
            break; \
        case AOP_TEQ: /* TEQ */ \
            result = a ^ b; \
            Rd_writeback = 0; \
            break; \
        case AOP_CMP: /* CMP */ \
            result = do_add(a, ~b, 1, &carry, &ovl); \
            Rd_writeback = 0; \
            arith_op = 1; \
            break; \
        case AOP_CMN: /* CMN */ \
            result = do_add(a, b, 0, &carry, &ovl); \
            Rd_writeback = 0; \
            arith_op = 1; \
            break; \
        case AOP_ORR: /* ORR */ \
            result = a | b; \
            break; \
        case AOP_MOV: /* MOV */ \
            result = b; \
            break; \
        case AOP_BIC: /* BIC */ \
            result = a & (~b); \
            break; \
        case AOP_MVN: /* MVN */ \
            result = ~b; \
            break; \
    }

#define DATA_PROCESSING_OP_TABLE_NOFLAGS(opcode, result, a, b) \
    switch(opcode) { \
        case AOP_AND: /* AND */ \
            result = a & b; \
            break; \
        case AOP_EOR: /* EOR */ \
            result = a ^ b; \
            break; \
        case AOP_SUB: /* SUB */ \
            result = a - b; \
            break; \
        case AOP_RSB: /* RSB */ \
            result = b - a; \
            break; \
        case AOP_ADD: /* ADD */ \
            result = a + b; \
            break; \
        case AOP_ADC: /* ADC */ \
            result = a + b + (get_condition(PSR_CC_CARRY) ? 1 : 0); \
            break; \
        case AOP_SBC: /* SBC */ \
            result = a + ~b + (get_condition(PSR_CC_CARRY) ? 1: 0); \
            break; \
        case AOP_RSC: /* RSC */ \
            result = b + ~a + (get_condition(PSR_CC_CARRY) ? 1: 0); \
            break; \
        case AOP_TST: /* TST */ \
            result = a & b; \
            break; \
        case AOP_TEQ: /* TEQ */ \
            result = a ^ b; \
            break; \
        case AOP_CMP: /* CMP */ \
            result = a - b; \
            break; \
        case AOP_CMN: /* CMN */ \
            result = a + b; \
            break; \
        case AOP_ORR: /* ORR */ \
            result = a | b; \
            break; \
        case AOP_MOV: /* MOV */ \
            result = b; \
            break; \
        case AOP_BIC: /* BIC */ \
            result = a & (~b); \
            break; \
        case AOP_MVN: /* MVN */ \
            result = ~b; \
            break; \
    }
/*================================================================*/
void uop_decode_me_arm(struct uop *op);
void uop_decode_me_arm(struct uop *op)
{
    // call the arm decoder and set the pc back to retry this instruction
    ASSERT(cpu.cp_pc != NULL);
    UOP_TRACE(6, "decoding arm opcode 0x%08x at pc 0x%x\n", op->undecoded.raw_instruction, cpu.pc);
    arm_decode_into_uop(op);
    cpu.pc -= 4; // back the instruction pointer up to retry this instruction
    cpu.cp_pc--;
    pfm_cnt_ins();
}

void uop_decode_me_thumb(struct uop *op);
void uop_decode_me_thumb(struct uop *op)
{
    // call the arm decoder and set the pc back to retry this instruction
    ASSERT(cpu.cp_pc != NULL);
    UOP_TRACE(6, "decoding thumb opcode 0x%04x at pc 0x%x\n", op->undecoded.raw_instruction, cpu.pc);
    thumb_decode_into_uop(op);
    cpu.pc -= 2; // back the instruction pointer up to retry this instruction
    cpu.cp_pc--;
    pfm_cnt_ins();
}

/*================================================================*/
void uop_b_immediate(struct uop *op);
void uop_b_immediate(struct uop *op)
{
    // branch to a fixed location outside of the current codepage.
    // Any offsets would have been resolved at decode time.
    if (op->flags & UOPBFLAGS_LINK) {
        int thumb = get_condition(PSR_THUMB) ? 1 : 0;
        put_reg(LR, op->b_immediate.link_target | thumb);
    }

    if (op->flags & UOPBFLAGS_SETTHUMB_ALWAYS) {
        set_condition(PSR_THUMB, TRUE);
        // force a codepage reload
        cdp.curr_cp = NULL;
    }
    if (op->flags & UOPBFLAGS_UNSETTHUMB_ALWAYS) {
        set_condition(PSR_THUMB, FALSE);
        // force a codepage reload
        cdp.curr_cp = NULL;
    }

    cpu.pc = op->b_immediate.target;
    if (likely(op->b_immediate.target_cp != NULL)) {
        // we have already cached a pointer to the target codepage, use it
        cdp.curr_cp = op->b_immediate.target_cp;
        cpu.cp_pc = PC_TO_CPPC(cpu.pc);
    } else {
        // see if we can lookup the target codepage and try again
        struct uop_codepage *cp = lookup_codepage(cpu.pc, get_condition(PSR_THUMB) ? TRUE : FALSE);
        if (cp != NULL) {
            // found one, cache it and set the code page
            op->b_immediate.target_cp = cp;
            cdp.curr_cp = cp;
            cpu.cp_pc = PC_TO_CPPC(cpu.pc);
        } else {
            // didn't find one, force a codepage reload next instruction
            cdp.curr_cp = NULL;
        }
    }


    pfm_cnt_op_b(op);
    pfm_cyc_op_b(op);

}

void uop_b_immediate_local(struct uop *op);
void uop_b_immediate_local(struct uop *op)
{
    // branch to a fixed location within the current codepage.
    if (op->flags & UOPBFLAGS_LINK) {
        int thumb = get_condition(PSR_THUMB) ? 1 : 0;
        put_reg(LR, op->b_immediate.link_target | thumb);
    }

    cpu.pc = op->b_immediate.target;
    ASSERT(cdp.curr_cp != NULL);
    cpu.cp_pc = PC_TO_CPPC(cpu.pc);

    pfm_cnt_op_b(op);
    pfm_cyc_op_b(op);
}

void uop_b_reg(struct uop *op);
void uop_b_reg(struct uop *op)
{
    armaddr_t temp_addr;

    temp_addr = get_reg(op->b_reg.reg);

    // branch to register contents
    if (op->flags & UOPBFLAGS_LINK) {
        int thumb = get_condition(PSR_THUMB) ? 1 : 0;
        put_reg(LR, (get_reg(PC) + op->b_reg.link_offset) | thumb);
    }

    put_reg(PC, temp_addr & 0xfffffffe);

    if ((temp_addr >> MMU_PAGESIZE_SHIFT) == (cpu.pc >> MMU_PAGESIZE_SHIFT)) {
        // it's a local branch, just recalc the position in the current codepage
        cpu.pc = temp_addr & 0xfffffffe;
        cpu.cp_pc = PC_TO_CPPC(cpu.pc);
    } else {
        // it's a remote branch
        cpu.pc = temp_addr & 0xfffffffe;
        cdp.curr_cp = NULL;
    }

    if (op->flags & UOPBFLAGS_SETTHUMB_ALWAYS) {
        set_condition(PSR_THUMB, TRUE);
        // force a codepage reload
        cdp.curr_cp = NULL;
    }
    if (op->flags & UOPBFLAGS_UNSETTHUMB_ALWAYS) {
        set_condition(PSR_THUMB, FALSE);
        // force a codepage reload
        cdp.curr_cp = NULL;
    }

    // if the bottom bit of the target address is 1, switch to thumb, otherwise switch to arm
    if (op->flags & UOPBFLAGS_SETTHUMB_COND) {
        bool old_condition = get_condition(PSR_THUMB) ? TRUE : FALSE;
        bool new_condition = (temp_addr & 1) ? TRUE : FALSE;

        if (old_condition != new_condition) {
            set_condition(PSR_THUMB, new_condition);
            // force a codepage reload
            cdp.curr_cp = NULL;
            UOP_TRACE(7, "B_REG: setting thumb to %d (new mode)\n", new_condition);
        }
    }

    pfm_cnt_op_b(op);
    pfm_cyc_op_b(op);
}

void uop_b_reg_offset(struct uop *op);
void uop_b_reg_offset(struct uop *op)
{
    armaddr_t temp_addr;

    temp_addr = get_reg(op->b_reg_offset.reg);
    temp_addr += op->b_reg_offset.offset;

    // branch to register contents
    if (op->flags & UOPBFLAGS_LINK) {
        int thumb = get_condition(PSR_THUMB) ? 1 : 0;
        put_reg(LR, (get_reg(PC) + op->b_reg_offset.link_offset) | thumb);
    }

    put_reg(PC, temp_addr & 0xfffffffe);

    if ((temp_addr >> MMU_PAGESIZE_SHIFT) == (cpu.pc >> MMU_PAGESIZE_SHIFT)) {
        // it's a local branch, just recalc the position in the current codepage
        cpu.pc = temp_addr & 0xfffffffe;
        cpu.cp_pc = PC_TO_CPPC(cpu.pc);
    } else {
        // it's a remote branch
        cpu.pc = temp_addr & 0xfffffffe;
        cdp.curr_cp = NULL;
    }

    if (op->flags & UOPBFLAGS_SETTHUMB_ALWAYS) {
        set_condition(PSR_THUMB, TRUE);
        // force a codepage reload
        cdp.curr_cp = NULL;
    }
    if (op->flags & UOPBFLAGS_UNSETTHUMB_ALWAYS) {
        set_condition(PSR_THUMB, FALSE);
        // force a codepage reload
        cdp.curr_cp = NULL;
    }

    // if the bottom bit of the target address is 1, switch to thumb, otherwise switch to arm
    if (op->flags & UOPBFLAGS_SETTHUMB_COND) {
        bool old_condition = get_condition(PSR_THUMB) ? TRUE : FALSE;
        bool new_condition = (temp_addr & 1) ? TRUE : FALSE;

        if (old_condition != new_condition) {
            set_condition(PSR_THUMB, new_condition);
            // force a codepage reload
            cdp.curr_cp = NULL;
            UOP_TRACE(7, "B_REG: setting thumb to %d (new mode)\n", new_condition);
        }
    }

    pfm_cnt_op_b(op);
    pfm_cyc_op_b(op);
}

/*================================================================*/
void uop_load_immediate_dword(struct uop *op);
void uop_load_immediate_dword(struct uop *op)
{
    dword temp_dword;

    // a very simple load, the address is already precalculated
    if (mmu_read_mem_dword(op->load_immediate.address, &temp_dword))
        return;

    // XXX on armv5 this can switch to thumb
    put_reg(op->load_immediate.target_reg, temp_dword);

    pfm_cnt_op_ld(op);
    pfm_cyc_op_ld_im_dword(op);
}

void uop_load_immediate_word(struct uop *op);
void uop_load_immediate_word(struct uop *op)
{
    word temp_word;
    dword temp_dword;

    // a very simple load, the address is already precalculated
    if (mmu_read_mem_word(op->load_immediate.address, &temp_word))
        return;
    temp_dword = temp_word;
    if (op->flags & UOPLSFLAGS_SIGN_EXTEND)
        temp_dword = SIGN_EXTEND(temp_dword, 15);

    // XXX on armv5 this can switch to thumb
    put_reg(op->load_immediate.target_reg, temp_dword);

    pfm_cnt_op_ld(op);
    pfm_cyc_op_ld_im_word(op);
}

void uop_load_immediate_byte(struct uop *op);
void uop_load_immediate_byte(struct uop *op)
{
    byte temp_byte;
    dword temp_dword;

    // a very simple load, the address is already precalculated
    if (mmu_read_mem_byte(op->load_immediate.address, &temp_byte))
        return;
    temp_dword = temp_byte;
    if (op->flags & UOPLSFLAGS_SIGN_EXTEND)
        temp_dword = SIGN_EXTEND(temp_dword, 7);

    // XXX on armv5 this can switch to thumb
    put_reg(op->load_immediate.target_reg, temp_dword);

    pfm_cnt_op_ld(op);
    pfm_cyc_op_ld_im_byte(op);
}

void uop_load_immediate_offset(struct uop *op);
void uop_load_immediate_offset(struct uop *op)
{
    armaddr_t temp_addr, temp_addr2, temp_addr3;
    dword temp_dword;

    // slightly more complex, add an offset to a register
    temp_addr2 = get_reg(op->load_store_immediate_offset.source_reg);
    temp_addr3 = temp_addr2 + op->load_store_immediate_offset.offset;

    if (op->flags & UOPLSFLAGS_POSTINDEX)
        temp_addr = temp_addr2; // use the pre-offset computed address
    else
        temp_addr = temp_addr3;

    // read in the difference sizes & sign extend
    switch (op->flags & UOPLSFLAGS_SIZE_MASK) {
        case UOPLSFLAGS_SIZE_DWORD:
            if (mmu_read_mem_dword(temp_addr, &temp_dword))
                return;
            break;
        case UOPLSFLAGS_SIZE_WORD: {
            word temp_word;

            if (mmu_read_mem_word(temp_addr, &temp_word))
                return;

            temp_dword = temp_word;
            if (op->flags & UOPLSFLAGS_SIGN_EXTEND)
                temp_dword = SIGN_EXTEND(temp_dword, 15);
            break;
        }
        case UOPLSFLAGS_SIZE_BYTE: {
            byte temp_byte;

            if (mmu_read_mem_byte(temp_addr, &temp_byte))
                return;
            temp_dword = temp_byte;
            if (op->flags & UOPLSFLAGS_SIGN_EXTEND)
                temp_dword = SIGN_EXTEND(temp_dword, 7);
            break;
        }
        case UOPLSFLAGS_SIZE_LONG: {
            ASSERT((op->load_store_scaled_reg_offset.target_reg & 1) == 0);

            // handle the first dword
            if (mmu_read_mem_dword(temp_addr, &temp_dword))
                return;

            put_reg(op->load_store_scaled_reg_offset.target_reg, temp_dword);

            // read the second dword
            dword temp_dword2;

            if (mmu_read_mem_dword(temp_addr + 4, &temp_dword2))
                return;

            // NOTE: if second register is r15, unpredictable
            put_reg(op->load_store_scaled_reg_offset.target_reg + 1, temp_dword2);
            break;
        }
    }

    // store the result
    // XXX on armv5 this can switch to thumb
    put_reg(op->load_store_immediate_offset.target_reg, temp_dword);

    // do writeback
    if (op->flags & UOPLSFLAGS_WRITEBACK)
        put_reg(op->load_store_immediate_offset.source_reg, temp_addr3);

    pfm_cnt_op_ld(op);
    pfm_cyc_op_ld_im_offset(op);
}

void uop_load_scaled_reg_offset(struct uop *op);
void uop_load_scaled_reg_offset(struct uop *op)
{
    armaddr_t temp_addr, temp_addr2, temp_addr3;
    dword temp_dword;

    // pretty complex. take two registers, optionally perform a shift operation on the second one,
    // add them together and then load that address
    temp_addr2 = get_reg(op->load_store_scaled_reg_offset.source_reg);
    temp_addr3 = get_reg(op->load_store_scaled_reg_offset.source2_reg);
    switch (op->load_store_scaled_reg_offset.shift_op) {
        case 0: // LSL
            temp_addr3 = LSL(temp_addr3, op->load_store_scaled_reg_offset.shift_immediate);
            break;
        case 1: // LSR
            if (op->load_store_scaled_reg_offset.shift_immediate == 0)
                temp_addr3 = 0;
            else
                temp_addr3 = LSR(temp_addr3, op->load_store_scaled_reg_offset.shift_immediate);
            break;
        case 2: // ASR
            if (op->load_store_scaled_reg_offset.shift_immediate == 0) {
                if (temp_addr3 & 0x80000000)
                    temp_addr3 = 0xffffffff;
                else
                    temp_addr3 = 0;
            } else {
                temp_addr3 = ASR(temp_addr3, op->load_store_scaled_reg_offset.shift_immediate);
            }
            break;
        case 3: // ROR or RRX
            if (op->load_store_scaled_reg_offset.shift_immediate == 0) { // RRX
                temp_addr3 = (cpu.cpsr ? 0x80000000 : 0) | LSR(temp_dword, 1);
            } else {
                temp_addr3 = ROR(temp_addr3, op->load_store_scaled_reg_offset.shift_immediate);
            }
            break;
    }

    if (op->flags & UOPLSFLAGS_NEGATE_OFFSET)
        temp_addr3 = -temp_addr3;

    temp_addr3 += temp_addr2; // fully calculated address

    if (op->flags & UOPLSFLAGS_POSTINDEX)
        temp_addr = temp_addr2; // use the pre-offset computed address
    else
        temp_addr = temp_addr3;

    // now we have an address, do the load
    // read in the difference sizes & sign extend
    switch (op->flags & UOPLSFLAGS_SIZE_MASK) {
        case UOPLSFLAGS_SIZE_DWORD:
            if (mmu_read_mem_dword(temp_addr, &temp_dword))
                return;
            break;
        case UOPLSFLAGS_SIZE_WORD: {
            word temp_word;

            if (mmu_read_mem_word(temp_addr, &temp_word))
                return;
            temp_dword = temp_word;
            if (op->flags & UOPLSFLAGS_SIGN_EXTEND)
                temp_dword = SIGN_EXTEND(temp_dword, 15);
            break;
        }
        case UOPLSFLAGS_SIZE_BYTE: {
            byte temp_byte;

            if (mmu_read_mem_byte(temp_addr, &temp_byte))
                return;
            temp_dword = temp_byte;
            if (op->flags & UOPLSFLAGS_SIGN_EXTEND)
                temp_dword = SIGN_EXTEND(temp_dword, 7);
            break;
        }
        case UOPLSFLAGS_SIZE_LONG: {
            ASSERT((op->load_store_scaled_reg_offset.target_reg & 1) == 0);

            // handle the first dword
            if (mmu_read_mem_dword(temp_addr, &temp_dword))
                return;

            put_reg(op->load_store_scaled_reg_offset.target_reg, temp_dword);

            // read the second dword
            dword temp_dword2;

            if (mmu_read_mem_dword(temp_addr + 4, &temp_dword2))
                return;

            // NOTE: if second register is r15, unpredictable
            put_reg(op->load_store_scaled_reg_offset.target_reg + 1, temp_dword2);
            break;
        }
    }

    // store the result
    // XXX on armv5 this can switch to thumb
    put_reg(op->load_store_scaled_reg_offset.target_reg, temp_dword);

    // do writeback
    if (op->flags & UOPLSFLAGS_WRITEBACK)
        put_reg(op->load_store_scaled_reg_offset.source_reg, temp_addr);

    pfm_cnt_op_ld(op);
    pfm_cyc_op_ld_reg_offset(op);
}

void uop_load_multiple(struct uop *op);
void uop_load_multiple(struct uop *op)
{
    armaddr_t temp_addr, temp_addr2;
    dword temp_dword;
    int i;
    dword reg_list = op->load_store_multiple.reg_bitmap;

    // calculate the base address
    temp_addr = get_reg(op->load_store_multiple.base_reg);
    temp_addr2 = temp_addr + op->load_store_multiple.base_offset;

    // scan through the list of registers, reading in each one
    ASSERT((reg_list >> 16) == 0);
    for (i = 0; reg_list != 0; i++, reg_list >>= 1) {
        if (reg_list & 1) {
            if (mmu_read_mem_dword(temp_addr2, &temp_dword)) {
                // there was a data abort, and we may have trashed the base register. Restore it.
                put_reg(op->load_store_multiple.base_reg, temp_addr);
                return;
            }

            // XXX on armv5 this can switch to thumb
            put_reg(i, temp_dword);
            temp_addr2 += 4;
        }
    }

    // writeback
    if (op->flags & UOPLSMFLAGS_WRITEBACK) {
        temp_addr2 = temp_addr + op->load_store_multiple.writeback_offset;
        put_reg(op->load_store_multiple.base_reg, temp_addr2);
    }

    // see if we need to move spsr into cpsr
    if (op->flags & UOPLSMFLAGS_LOAD_CPSR) {
        reg_t spsr = cpu.spsr; // save it here because cpu.spsr might change in set_cpu_mode()
        set_cpu_mode(cpu.spsr & PSR_MODE_MASK);
        cpu.cpsr = spsr;
    }

    pfm_cnt_op_ld(op);
    pfm_cyc_op_ld_multi(op);
}

void uop_load_multiple_s(struct uop *op);
void uop_load_multiple_s(struct uop *op)
{
    armaddr_t temp_addr, temp_addr2;
    dword temp_dword;
    int i;
    dword reg_list = op->load_store_multiple.reg_bitmap;

    // calculate the base address
    temp_addr = get_reg(op->load_store_multiple.base_reg);
    temp_addr2 = temp_addr + op->load_store_multiple.base_offset;

    // r15 cannot be in the register list, would have resulted in a different instruction
    ASSERT((reg_list & 0x8000) == 0);

    // scan through the list of registers, reading in each one
    ASSERT((reg_list >> 16) == 0);
    for (i = 0; reg_list != 0; i++, reg_list >>= 1) {
        if (reg_list & 1) {
            if (mmu_read_mem_dword(temp_addr2, &temp_dword)) {
                // there was a data abort, and we may have trashed the base register. Restore it.
                put_reg_user(op->load_store_multiple.base_reg, temp_addr);
                return;
            }
            put_reg_user(i, temp_dword);
            temp_addr2 += 4;
        }
    }

    // writeback
    // NOTE: writeback with the S bit set is unpredictable
    if (op->flags & UOPLSMFLAGS_WRITEBACK) {
        temp_addr2 = temp_addr + op->load_store_multiple.writeback_offset;
        put_reg(op->load_store_multiple.base_reg, temp_addr2);
    }

    pfm_cnt_op_ld(op);
    pfm_cyc_op_ld_multi_s(op);
}
/*================================================================*/
void uop_store_immediate_offset(struct uop *op);
void uop_store_immediate_offset(struct uop *op)
{
    armaddr_t temp_addr, temp_addr2, temp_addr3;
    dword temp_dword;

    // slightly more complex, add an offset to a register
    temp_addr2 = get_reg(op->load_store_immediate_offset.source_reg);
    temp_addr3 = temp_addr2 + op->load_store_immediate_offset.offset;

    if (op->flags & UOPLSFLAGS_POSTINDEX)
        temp_addr = temp_addr2; // use the pre-offset computed address
    else
        temp_addr = temp_addr3;

    // read in what we're going to store
    temp_dword = get_reg(op->load_store_immediate_offset.target_reg);

    // write it out based on the size we were requested
    switch (op->flags & UOPLSFLAGS_SIZE_MASK) {
        case UOPLSFLAGS_SIZE_DWORD:
            if (mmu_write_mem_dword(temp_addr, temp_dword))
                return;
            break;
        case UOPLSFLAGS_SIZE_WORD:
            if (mmu_write_mem_word(temp_addr, (word)temp_dword))
                return;
            break;
        case UOPLSFLAGS_SIZE_BYTE:
            if (mmu_write_mem_byte(temp_addr, (byte)temp_dword))
                return;
            break;
        case UOPLSFLAGS_SIZE_LONG:
            ASSERT((op->load_store_scaled_reg_offset.target_reg & 1) == 0);

            // handle the first dword
            if (mmu_write_mem_dword(temp_addr, temp_dword))
                return;

            // read the second dword
            temp_dword = get_reg(op->load_store_scaled_reg_offset.target_reg + 1);

            if (mmu_write_mem_dword(temp_addr + 4, temp_dword))
                return;
            break;
    }

    // do writeback
    if (op->flags & UOPLSFLAGS_WRITEBACK)
        put_reg(op->load_store_immediate_offset.source_reg, temp_addr3);

    pfm_cnt_op_st(op);
    pfm_cyc_op_st_im_offset(op);
}

void uop_store_scaled_reg_offset(struct uop *op);
void uop_store_scaled_reg_offset(struct uop *op)
{
    armaddr_t temp_addr, temp_addr2, temp_addr3;
    dword temp_dword;

    // pretty complex. take two registers, optionally perform a shift operation on the second one,
    // add them together and then load that address
    // XXX room for improvement here, since lots of times I'm sure an instruction
    // decoded to a immediate shift of zero to get plain register add
    temp_addr2 = get_reg(op->load_store_scaled_reg_offset.source_reg);
    temp_addr3 = get_reg(op->load_store_scaled_reg_offset.source2_reg);
    switch (op->load_store_scaled_reg_offset.shift_op) {
        case 0: // LSL
            temp_addr3 = LSL(temp_addr3, op->load_store_scaled_reg_offset.shift_immediate);
            break;
        case 1: // LSR
            if (op->load_store_scaled_reg_offset.shift_immediate == 0)
                temp_addr3 = 0;
            else
                temp_addr3 = LSR(temp_addr3, op->load_store_scaled_reg_offset.shift_immediate);
            break;
        case 2: // ASR
            if (op->load_store_scaled_reg_offset.shift_immediate == 0) {
                if (temp_addr3 & 0x80000000)
                    temp_addr3 = 0xffffffff;
                else
                    temp_addr3 = 0;
            } else {
                temp_addr3 = ASR(temp_addr3, op->load_store_scaled_reg_offset.shift_immediate);
            }
            break;
        case 3: // ROR or RRX
            if (op->load_store_scaled_reg_offset.shift_immediate == 0) { // RRX
                temp_addr3 = (cpu.cpsr ? 0x80000000 : 0) | LSR(temp_addr3, 1);
            } else {
                temp_addr3 = ROR(temp_addr3, op->load_store_scaled_reg_offset.shift_immediate);
            }
            break;
    }

    if (op->flags & UOPLSFLAGS_NEGATE_OFFSET)
        temp_addr3 = -temp_addr3;

    temp_addr3 += temp_addr2; // fully calculated address

    if (op->flags & UOPLSFLAGS_POSTINDEX)
        temp_addr = temp_addr2; // use the pre-offset computed address
    else
        temp_addr = temp_addr3;

    // read in what we're going to store
    temp_dword = get_reg(op->load_store_scaled_reg_offset.target_reg);

    // write it out based on the size we were requested
    switch (op->flags & UOPLSFLAGS_SIZE_MASK) {
        case UOPLSFLAGS_SIZE_DWORD:
            if (mmu_write_mem_dword(temp_addr, temp_dword))
                return;
            break;
        case UOPLSFLAGS_SIZE_WORD:
            if (mmu_write_mem_word(temp_addr, (word)temp_dword))
                return;
            break;
        case UOPLSFLAGS_SIZE_BYTE:
            if (mmu_write_mem_byte(temp_addr, (byte)temp_dword))
                return;
            break;
        case UOPLSFLAGS_SIZE_LONG:
            ASSERT((op->load_store_scaled_reg_offset.target_reg & 1) == 0);

            // handle the first dword
            if (mmu_write_mem_dword(temp_addr, temp_dword))
                return;

            // read the second dword
            temp_dword = get_reg(op->load_store_scaled_reg_offset.target_reg + 1);

            if (mmu_write_mem_dword(temp_addr + 4, temp_dword))
                return;
            break;
    }

    // do writeback
    if (op->flags & UOPLSFLAGS_WRITEBACK)
        put_reg(op->load_store_scaled_reg_offset.source_reg, temp_addr);

    pfm_cnt_op_st(op);
    pfm_cyc_op_st_reg_offset(op);
}


void uop_store_multiple(struct uop *op);
void uop_store_multiple(struct uop *op)
{
    armaddr_t temp_addr, temp_addr2;
    int i;
    dword reg_list = op->load_store_multiple.reg_bitmap;

    // calculate the base address
    temp_addr = get_reg(op->load_store_multiple.base_reg);
    temp_addr2 = temp_addr + op->load_store_multiple.base_offset;

    // scan through the list of registers, storing each one
    ASSERT((reg_list >> 16) == 0);
    for (i = 0; reg_list != 0; i++, reg_list >>= 1) {
        if (reg_list & 1) {
            if (mmu_write_mem_dword(temp_addr2, get_reg(i)))
                return; // data abort
            temp_addr2 += 4;
        }
    }

    // writeback
    if (op->flags & UOPLSMFLAGS_WRITEBACK) {
        temp_addr2 = temp_addr + op->load_store_multiple.writeback_offset;
        put_reg(op->load_store_multiple.base_reg, temp_addr2);
    }

    pfm_cnt_op_st(op);
    pfm_cyc_op_st_multi(op);
}

void uop_store_multiple_s(struct uop *op);
void uop_store_multiple_s(struct uop *op)
{
    armaddr_t temp_addr, temp_addr2;
    int i;
    dword reg_list = op->load_store_multiple.reg_bitmap;

    // calculate the base address
    temp_addr = get_reg(op->load_store_multiple.base_reg);
    temp_addr2 = temp_addr + op->load_store_multiple.base_offset;

    // scan through the list of registers, storing each one
    ASSERT((reg_list >> 16) == 0);
    for (i = 0; reg_list != 0; i++, reg_list >>= 1) {
        if (reg_list & 1) {
            if (mmu_write_mem_dword(temp_addr2, get_reg_user(i)))
                return; // data abort
            temp_addr2 += 4;
        }
    }

    // writeback
    // NOTE: writeback with the S bit set is unpredictable
    if (op->flags & UOPLSMFLAGS_WRITEBACK) {
        temp_addr2 = temp_addr + op->load_store_multiple.writeback_offset;
        put_reg(op->load_store_multiple.base_reg, temp_addr2);
    }

    pfm_cnt_op_st(op);
    pfm_cyc_op_st_multi_s(op);
}

/*================================================================*/
// generic data process with immediate operand, no S bit, PC may be target
void uop_data_processing_imm(struct uop *op);
void uop_data_processing_imm(struct uop *op)
{
    dword immediate = op->data_processing_imm.immediate;
    dword temp_dword = get_reg(op->data_processing_imm.source_reg);

    ASSERT(op->data_processing_imm.dp_opcode < 16);
    ASSERT_VALID_REG(op->data_processing_imm.source_reg);
    ASSERT_VALID_REG(op->data_processing_imm.dest_reg);

    DATA_PROCESSING_OP_TABLE_NOFLAGS(op->data_processing_imm.dp_opcode,
                                     temp_dword, // result
                                     temp_dword, immediate); // a & b

    // write the result out
    // NOTE: if the op was originally one of the four arm test ops
    // (TST, TEQ, CMP, CMN), we would be using the DATA_PROCESSING_IMM_S
    // instruction form, since not having writeback makes no sense
    put_reg(op->data_processing_imm.dest_reg, temp_dword);

    pfm_cnt_op_data(op);
    pfm_cnt_op_arith_imm(op);
}

// generic data processing with register operand, no S bit, PC may be target
void uop_data_processing_reg(struct uop *op);
void uop_data_processing_reg(struct uop *op)
{
    dword temp_dword = get_reg(op->data_processing_reg.source_reg);
    dword operand2 = get_reg(op->data_processing_reg.source2_reg);

    ASSERT(op->data_processing_reg.dp_opcode < 16);
    ASSERT_VALID_REG(op->data_processing_reg.source_reg);
    ASSERT_VALID_REG(op->data_processing_reg.source2_reg);
    ASSERT_VALID_REG(op->data_processing_reg.dest_reg);

    DATA_PROCESSING_OP_TABLE_NOFLAGS(op->data_processing_reg.dp_opcode,
                                     temp_dword, // result
                                     temp_dword, operand2); // a & b

    // write the result out
    // NOTE: if the op was originally one of the four arm test ops
    // (TST, TEQ, CMP, CMN), we would be using the DATA_PROCESSING_REG_S
    // instruction form, since not having writeback makes no sense
    put_reg(op->data_processing_reg.dest_reg, temp_dword);

    pfm_cnt_op_data(op);
    pfm_cnt_op_arith_reg(op);
}

// generic data process with immediate operand, S bit, PC may be target
void uop_data_processing_imm_s(struct uop *op);
void uop_data_processing_imm_s(struct uop *op)
{
    bool Rd_writeback;
    bool arith_op;
    dword immediate = op->data_processing_imm.immediate;
    int carry, ovl;

    dword temp_dword = get_reg(op->data_processing_imm.source_reg);

    ASSERT(op->data_processing_imm.dp_opcode < 16);
    ASSERT_VALID_REG(op->data_processing_imm.source_reg);
    ASSERT_VALID_REG(op->data_processing_imm.dest_reg);

    Rd_writeback = TRUE;
    arith_op = FALSE;
    carry = 0;
    ovl = 0;
    DATA_PROCESSING_OP_TABLE(op->data_processing_imm.dp_opcode,
                             temp_dword, // result
                             temp_dword, immediate, // a & b
                             arith_op, Rd_writeback, carry, ovl);

    if (Rd_writeback)
        put_reg(op->data_processing_imm.dest_reg, temp_dword);

    if (op->data_processing_imm.dest_reg != PC) {
        set_NZ_condition(temp_dword);
        if (arith_op) {
            set_condition(PSR_CC_CARRY, carry);
            set_condition(PSR_CC_OVL, ovl);
        } else {
            // carry out from the shifter depending on how it was precalculated
            if (op->flags & UOPDPFLAGS_SET_CARRY_FROM_SHIFTER)
                set_condition(PSR_CC_CARRY, op->flags & UOPDPFLAGS_CARRY_FROM_SHIFTER);
        }
    } else {
        // destination was pc, and S bit was set, this means we swap spsr
        reg_t spsr = cpu.spsr; // save it here because cpu.spsr might change in set_cpu_mode()

        // see if we're about to switch thumb state
        if ((spsr & PSR_THUMB) != (cpu.cpsr & PSR_THUMB))
            cdp.curr_cp = NULL; // force a codepage reload

        set_cpu_mode(cpu.spsr & PSR_MODE_MASK);
        cpu.cpsr = spsr;
    }

    pfm_cnt_op_data(op);
    pfm_cnt_op_arith_imm_shift(op);

}

// generic data processing with register operand, S bit, PC may be target
void uop_data_processing_reg_s(struct uop *op);
void uop_data_processing_reg_s(struct uop *op)
{
    bool Rd_writeback;
    bool arith_op;
    int carry, ovl;

    dword temp_dword = get_reg(op->data_processing_reg.source_reg);
    dword temp_dword2 = get_reg(op->data_processing_reg.source2_reg);

    ASSERT(op->data_processing_reg.dp_opcode < 16);
    ASSERT_VALID_REG(op->data_processing_reg.source_reg);
    ASSERT_VALID_REG(op->data_processing_reg.source2_reg);
    ASSERT_VALID_REG(op->data_processing_reg.dest_reg);

    Rd_writeback = TRUE;
    arith_op = FALSE;
    carry = 0;
    ovl = 0;
    DATA_PROCESSING_OP_TABLE(op->data_processing_reg.dp_opcode,
                             temp_dword, // result
                             temp_dword, temp_dword2, // a & b
                             arith_op, Rd_writeback, carry, ovl);

    if (Rd_writeback)
        put_reg(op->data_processing_reg.dest_reg, temp_dword);

    if (op->data_processing_reg.dest_reg != PC) {
        set_NZ_condition(temp_dword);
        if (arith_op) {
            set_condition(PSR_CC_CARRY, carry);
            set_condition(PSR_CC_OVL, ovl);
        }
    } else {
        // destination was pc, and S bit was set, this means we swap spsr
        reg_t spsr = cpu.spsr; // save it here because cpu.spsr might change in set_cpu_mode()

        // see if we're about to switch thumb state
        if ((spsr & PSR_THUMB) != (cpu.cpsr & PSR_THUMB))
            cdp.curr_cp = NULL; // force a codepage reload

        set_cpu_mode(cpu.spsr & PSR_MODE_MASK);
        cpu.cpsr = spsr;
    }

    pfm_cnt_op_data(op);
    pfm_cnt_op_arith_reg_shift(op);
}

// generic data processing with immediate barrel shifter, no S bit, PC may be target
void uop_data_processing_imm_shift(struct uop *op);
void uop_data_processing_imm_shift(struct uop *op)
{
    bool Rd_writeback;
    bool arith_op;
    int carry, ovl;
    bool shifter_carry_out;
    dword shifter_operand;
    dword shift_imm;

    ASSERT(op->data_processing_imm_shift.shift_opcode < 4);
    ASSERT(op->data_processing_imm_shift.dp_opcode < 16);
    ASSERT_VALID_REG(op->data_processing_imm_shift.source_reg);
    ASSERT_VALID_REG(op->data_processing_imm_shift.source2_reg);
    ASSERT_VALID_REG(op->data_processing_imm_shift.dest_reg);

    // first operand
    dword temp_dword = get_reg(op->data_processing_imm_shift.source_reg);
    dword temp_dword2 = get_reg(op->data_processing_imm_shift.source2_reg);
    shift_imm = op->data_processing_imm_shift.shift_imm;

    // handle the immediate shift form of barrel shifter
    switch (op->data_processing_imm_shift.shift_opcode) {
        default:
        case 0: // LSL
            if (shift_imm == 0) {
                // shouldn't see this form, it would have been factored out into a simpler instruction
                shifter_operand = temp_dword2;
                shifter_carry_out = get_condition(PSR_CC_CARRY);
            } else {
                shifter_operand = LSL(temp_dword2, shift_imm);
                shifter_carry_out = BIT(temp_dword2, 32 - shift_imm);
            }
            break;
        case 1: // LSR
            if (shift_imm == 0) {
                shifter_operand = 0;
                shifter_carry_out = BIT(temp_dword2, 31);
            } else {
                shifter_operand = LSR(temp_dword2, shift_imm);
                shifter_carry_out = BIT(temp_dword2, shift_imm - 1);
            }
            break;
        case 2: // ASR
            if (shift_imm == 0) {
                if (BIT(temp_dword2, 31) == 0) {
                    shifter_operand = 0;
                    shifter_carry_out = 0; // Rm[31] == 0
                } else {
                    shifter_operand = 0xffffffff;
                    shifter_carry_out = 0x80000000; // Rm[31] == 1
                }
            } else {
                shifter_operand = ASR(temp_dword2, shift_imm);
                shifter_carry_out = BIT(temp_dword2, shift_imm - 1);
            }
            break;
        case 3: // ROR
            if (shift_imm == 0) {
                // RRX
                shifter_operand = (get_condition(PSR_CC_CARRY) ? 0x80000000: 0) | LSR(temp_dword2, 1);
                shifter_carry_out = BIT(temp_dword2, 0);
            } else {
                shifter_operand = ROR(temp_dword2, shift_imm);
                shifter_carry_out = BIT(temp_dword2, shift_imm - 1);
            }
            break;
    }

    // do the op
    Rd_writeback = TRUE;
    arith_op = FALSE;
    carry = 0;
    ovl = 0;
    DATA_PROCESSING_OP_TABLE(op->data_processing_imm_shift.dp_opcode,
                             temp_dword, // result
                             temp_dword, shifter_operand, // a & b
                             arith_op, Rd_writeback, carry, ovl);

    if (Rd_writeback)
        put_reg(op->data_processing_imm_shift.dest_reg, temp_dword);

    if (op->flags & UOPDPFLAGS_S_BIT) {
        if (op->data_processing_imm_shift.dest_reg != PC) {
            set_NZ_condition(temp_dword);
            if (arith_op) {
                set_condition(PSR_CC_CARRY, carry);
                set_condition(PSR_CC_OVL, ovl);
            } else {
                // carry out from the shifter
                set_condition(PSR_CC_CARRY, shifter_carry_out);
            }
        } else {
            // destination was pc, and S bit was set, this means we swap spsr
            reg_t spsr = cpu.spsr; // save it here because cpu.spsr might change in set_cpu_mode()

            // see if we're about to switch thumb state
            if ((spsr & PSR_THUMB) != (cpu.cpsr & PSR_THUMB))
                cdp.curr_cp = NULL; // force a codepage reload

            set_cpu_mode(cpu.spsr & PSR_MODE_MASK);
            cpu.cpsr = spsr;
        }
    }

    pfm_cnt_op_data(op);
    pfm_cnt_op_arith_imm_shift(op);
}

// generic data processing with register based barrel shifter, no S bit, PC may be target
void uop_data_processing_reg_shift(struct uop *op);
void uop_data_processing_reg_shift(struct uop *op)
{
    bool Rd_writeback;
    bool arith_op;
    int carry, ovl;
    bool shifter_carry_out;
    dword shifter_operand;

    ASSERT(op->data_processing_reg_shift.shift_opcode < 4);
    ASSERT(op->data_processing_reg_shift.dp_opcode < 16);
    ASSERT_VALID_REG(op->data_processing_reg_shift.source_reg);
    ASSERT_VALID_REG(op->data_processing_reg_shift.source2_reg);
    ASSERT_VALID_REG(op->data_processing_reg_shift.dest_reg);

    // operands
    dword temp_dword = get_reg(op->data_processing_reg_shift.source_reg);
    dword temp_dword2 = get_reg(op->data_processing_reg_shift.source2_reg);
    dword temp_dword3 = get_reg(op->data_processing_reg_shift.shift_reg);

    // we only care about the bottom 8 bits of Rs
    temp_dword3 = BITS(temp_dword3, 7, 0);

    // handle the immediate shift form of barrel shifter
    switch (op->data_processing_reg_shift.shift_opcode) {
        default:
        case 0: // LSL by reg (page A5-10)
            shifter_operand = LSL(temp_dword2, temp_dword3);
            if (temp_dword3 == 0) {
                shifter_carry_out = get_condition(PSR_CC_CARRY);
            } else if (temp_dword3 < 32) {
                shifter_carry_out = BIT(temp_dword2, 32 - temp_dword3);
            } else if (temp_dword3 == 32) {
                shifter_carry_out = BIT(temp_dword2, 0);
            } else { // temp_dword3 > 32
                shifter_carry_out = 0;
            }
            break;
        case 1: // LSR by reg (page A5-12)
            shifter_operand = LSR(temp_dword2, temp_dword3);
            if (temp_dword3 == 0) {
                shifter_carry_out = get_condition(PSR_CC_CARRY);
            } else if (temp_dword3 < 32) {
                shifter_carry_out = BIT(temp_dword2, temp_dword3 - 1);
            } else if (temp_dword3 == 32) {
                shifter_carry_out = BIT(temp_dword2, 31);
            } else {
                shifter_carry_out = 0;
            }
            break;
        case 2: // ASR by reg (page A5-14)
            shifter_operand = ASR(temp_dword2, temp_dword3);
            if (temp_dword3 == 0) {
                shifter_carry_out = get_condition(PSR_CC_CARRY);
            } else if (temp_dword3 < 32) {
                shifter_carry_out = BIT(temp_dword2, temp_dword3 - 1);
            } else if (temp_dword3 >= 32) {
                shifter_carry_out = BIT(temp_dword2, 31);
            }
            break;
        case 3: { // ROR by reg (page A5-16)
            dword lower_4bits = BITS(temp_dword3, 4, 0);
            shifter_operand = ROR(temp_dword2, lower_4bits);
            if (temp_dword3 == 0) {
                shifter_carry_out = get_condition(PSR_CC_CARRY);
            } else if (lower_4bits == 0) {
                shifter_carry_out = BIT(temp_dword2, 31);
            } else { // temp_dword3 & 0x1f > 0
                shifter_carry_out = BIT(temp_dword2, lower_4bits - 1);
            }
            break;
        }
    }

    pfm_cyc_inc();

    // do the op
    Rd_writeback = TRUE;
    arith_op = FALSE;
    carry = 0;
    ovl = 0;
    DATA_PROCESSING_OP_TABLE(op->data_processing_reg_shift.dp_opcode,
                             temp_dword, // result
                             temp_dword, shifter_operand, // a & b
                             arith_op, Rd_writeback, carry, ovl);

    if (Rd_writeback)
        put_reg(op->data_processing_reg_shift.dest_reg, temp_dword);

    if (op->flags & UOPDPFLAGS_S_BIT) {
        if (op->data_processing_reg_shift.dest_reg != PC) {
            set_NZ_condition(temp_dword);
            if (arith_op) {
                set_condition(PSR_CC_CARRY, carry);
                set_condition(PSR_CC_OVL, ovl);
            } else {
                // carry out from the shifter
                set_condition(PSR_CC_CARRY, shifter_carry_out);
            }
        } else {
            // destination was pc, and S bit was set, this means we swap spsr
            reg_t spsr = cpu.spsr; // save it here because cpu.spsr might change in set_cpu_mode()

            // see if we're about to switch thumb state
            if ((spsr & PSR_THUMB) != (cpu.cpsr & PSR_THUMB))
                cdp.curr_cp = NULL; // force a codepage reload

            set_cpu_mode(cpu.spsr & PSR_MODE_MASK);
            cpu.cpsr = spsr;
        }
    }

    pfm_cnt_op_data(op);
    pfm_cnt_op_arith_reg_shift(op);
}

/*================================================================*/
// simple load of immediate into register, PC may not be target
void uop_mov_imm(struct uop *op);
void uop_mov_imm(struct uop *op)
{
    put_reg_nopc(op->simple_dp_imm.dest_reg, op->simple_dp_imm.immediate);

    pfm_cnt_op_data(op);
}

// simple load of immediate into register, set N and Z flags, PC may not be target
void uop_mov_imm_nz(struct uop *op);
void uop_mov_imm_nz(struct uop *op)
{
    put_reg_nopc(op->simple_dp_imm.dest_reg, op->simple_dp_imm.immediate);
    set_NZ_condition(op->simple_dp_imm.immediate);

    pfm_cnt_op_data(op);
}

// simple mov from register to register, PC may not be target
void uop_mov_reg(struct uop *op);
void uop_mov_reg(struct uop *op)
{
    put_reg_nopc(op->simple_dp_reg.dest_reg, get_reg(op->simple_dp_reg.source2_reg));

    pfm_cnt_op_data(op);
}

/*================================================================*/
// simple compare of register to immediate value
void uop_cmp_imm_s(struct uop *op);
void uop_cmp_imm_s(struct uop *op)
{
    int carry, ovl;
    dword result;

    // subtract the immediate from the source register
    result = do_add(get_reg(op->simple_dp_imm.source_reg), ~(op->simple_dp_imm.immediate), 1, &carry, &ovl);

    // set flags on the result
    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);

    pfm_cnt_op_data(op);
}

// simple compare of two registers
void uop_cmp_reg_s(struct uop *op);
void uop_cmp_reg_s(struct uop *op)
{
    int carry, ovl;
    dword result;

    // subtract the source2 reg from the source register
    result = do_add(get_reg(op->simple_dp_reg.source_reg), ~(get_reg(op->simple_dp_reg.source2_reg)), 1, &carry, &ovl);

    // set flags on the result
    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);

    pfm_cnt_op_data(op);
}

// simple negative compare of two registers
void uop_cmn_reg_s(struct uop *op);
void uop_cmn_reg_s(struct uop *op)
{
    int carry, ovl;
    dword result;

    // subtract the source2 reg from the source register
    result = do_add(get_reg(op->simple_dp_reg.source_reg), get_reg(op->simple_dp_reg.source2_reg), 0, &carry, &ovl);

    // set flags on the result
    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);

    pfm_cnt_op_data(op);
}


// bit test of two registers
void uop_tst_reg_s(struct uop *op);
void uop_tst_reg_s(struct uop *op)
{
    dword result;
    dword a;
    dword b;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);

    result = a & b;

    // set flags on the result
    set_NZ_condition(result);

    pfm_cnt_op_data(op);
}

/*================================================================*/
// simple add of immediate to register, PC may not be target
void uop_add_imm(struct uop *op);
void uop_add_imm(struct uop *op)
{
    dword a;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    result = a + op->simple_dp_imm.immediate;
    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// simple add of immediate to register, S bit, PC may not be target
void uop_add_imm_s(struct uop *op);
void uop_add_imm_s(struct uop *op)
{
    int carry, ovl;
    dword a;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    result = do_add(a, op->simple_dp_imm.immediate, 0, &carry, &ovl);
    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    // set flags on the result
    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);

    pfm_cnt_op_data(op);
}

// simple add of two registers, PC may not be target
void uop_add_reg(struct uop *op);
void uop_add_reg(struct uop *op)
{
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = a + b;
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// simple add of two registers, S bit, PC may not be target
void uop_add_reg_s(struct uop *op);
void uop_add_reg_s(struct uop *op)
{
    int carry, ovl;
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = do_add(a, b, 0, &carry, &ovl);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    // set flags on the result
    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);

    pfm_cnt_op_data(op);
}

// simple add with carry of two registers, S bit, PC may not be target
void uop_adc_reg_s(struct uop *op);
void uop_adc_reg_s(struct uop *op)
{
    int carry, ovl;
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = do_add(a, b, get_condition(PSR_CC_CARRY) ? 1 : 0, &carry, &ovl);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    // set flags on the result
    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);

    pfm_cnt_op_data(op);
}

// simple subtract of two registers, S bit, PC may not be target
void uop_sub_reg_s(struct uop *op);
void uop_sub_reg_s(struct uop *op)
{
    int carry, ovl;
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = do_add(a, ~b, 1, &carry, &ovl);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    // set flags on the result
    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);

    pfm_cnt_op_data(op);
}

// simple subtract with carry of two registers, S bit, PC may not be target
void uop_sbc_reg_s(struct uop *op);
void uop_sbc_reg_s(struct uop *op)
{
    int carry, ovl;
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = do_add(a, ~b, get_condition(PSR_CC_CARRY) ? 1 : 0, &carry, &ovl);
    put_reg(op->simple_dp_reg.dest_reg, result);

    // set flags on the result
    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);

    pfm_cnt_op_data(op);
}

/*================================================================*/
// and with immediate, PC may not be target
void uop_and_imm(struct uop *op);
void uop_and_imm(struct uop *op)
{
    dword a;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    result = a & op->simple_dp_imm.immediate;

    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// or with immediate, PC may not be target
void uop_orr_imm(struct uop *op);
void uop_orr_imm(struct uop *op)
{
    dword a;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    result = a | op->simple_dp_imm.immediate;

    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// or by register, S bit, PC may not be target
void uop_orr_reg_s(struct uop *op);
void uop_orr_reg_s(struct uop *op)
{
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = a | b;

    set_NZ_condition(result);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

/*================================================================*/
// shift left of register by immediate, PC may not be target
void uop_lsl_imm(struct uop *op);
void uop_lsl_imm(struct uop *op)
{
    dword a;
    dword shift;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    shift = op->simple_dp_imm.immediate;

    result = LSL(a, shift);

    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// shift left of register by immediate, S bit, PC may not be target
void uop_lsl_imm_s(struct uop *op);
void uop_lsl_imm_s(struct uop *op)
{
    int carry;
    dword a;
    dword immed;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    immed = op->simple_dp_imm.immediate;

    if (immed != 0) {
        carry = BIT(a, 32 - immed);
        result = LSL(a, immed);
    } else {
        carry = get_condition(PSR_CC_CARRY);
        result = a;
    }

    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// shift left of register by register, PC may not be target
void uop_lsl_reg(struct uop *op);
void uop_lsl_reg(struct uop *op)
{
    dword a;
    dword shift;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    shift = get_reg(op->simple_dp_reg.source2_reg);
    shift &= 0xff;

    result = LSL(a, shift);

    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// shift left of register by register, S bit, PC may not be target
void uop_lsl_reg_s(struct uop *op);
void uop_lsl_reg_s(struct uop *op)
{
    int carry;
    dword a;
    dword shift;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    shift = get_reg(op->simple_dp_reg.source2_reg);
    shift &= 0xff;

    result = LSL(a, shift);
    if (shift == 0) {
        carry = get_condition(PSR_CC_CARRY);
    } else if (shift < 32) {
        carry = BIT(a, 32 - shift);
    } else if (shift == 32) {
        carry = BIT(a, 0);
    } else {
        carry = 0;
    }

    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// logical shift right by immediate, PC may not be target
void uop_lsr_imm(struct uop *op);
void uop_lsr_imm(struct uop *op)
{
    dword a;
    dword immed;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    immed = op->simple_dp_imm.immediate;

    result = LSR(a, immed);

    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// logical shift right by immediate, S bit, PC may not be target
void uop_lsr_imm_s(struct uop *op);
void uop_lsr_imm_s(struct uop *op)
{
    int carry;
    dword a;
    dword immed;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    immed = op->simple_dp_imm.immediate;

    if (immed != 0) {
        carry = BIT(a, immed - 1);
        result = LSR(a, immed);
    } else {
        carry = BIT(a, 31);
        result = 0;
    }

    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// logical shift right by register, PC may not be target
void uop_lsr_reg(struct uop *op);
void uop_lsr_reg(struct uop *op)
{
    dword a;
    dword shift;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    shift = get_reg(op->simple_dp_reg.source2_reg);
    shift &= 0xff;

    result = LSR(a, shift);

    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// logical shift right by register, S bit, PC may not be target
void uop_lsr_reg_s(struct uop *op);
void uop_lsr_reg_s(struct uop *op)
{
    int carry;
    dword a;
    dword shift;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    shift = get_reg(op->simple_dp_reg.source2_reg);
    shift &= 0xff;

    result = LSR(a, shift);
    if (shift == 0) {
        carry = get_condition(PSR_CC_CARRY);
    } else if (shift < 32) {
        carry = BIT(a, shift - 1);
    } else if (shift == 32) {
        carry = BIT(a, 31);
    } else {
        carry = 0;
    }

    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// arithmetic shift right by immediate,  PC may not be target
void uop_asr_imm(struct uop *op);
void uop_asr_imm(struct uop *op)
{
    dword a;
    dword immed;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    immed = op->simple_dp_imm.immediate;

    result = ASR(a, immed);

    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// arithmetic shift right by immediate, S bit, PC may not be target
void uop_asr_imm_s(struct uop *op);
void uop_asr_imm_s(struct uop *op)
{
    int carry;
    dword a;
    dword immed;
    dword result;

    a = get_reg(op->simple_dp_imm.source_reg);
    immed = op->simple_dp_imm.immediate;

    if (immed == 0) {
        carry = BIT(a, 31);
        if (carry)
            result = 0;
        else
            result = 0xffffffff;
    } else {
        carry = BIT(a, immed - 1);
        result = ASR(a, immed);
    }

    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    put_reg_nopc(op->simple_dp_imm.dest_reg, result);

    pfm_cnt_op_data(op);
}

// arithmetic shift right by register, S bit, PC may not be target
void uop_asr_reg(struct uop *op);
void uop_asr_reg(struct uop *op)
{
    dword a;
    dword shift;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    shift = get_reg(op->simple_dp_reg.source2_reg);
    shift &= 0xff;

    result = ASR(a, shift);

    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// arithmetic shift right by register, S bit, PC may not be target
void uop_asr_reg_s(struct uop *op);
void uop_asr_reg_s(struct uop *op)
{
    int carry;
    dword a;
    dword shift;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    shift = get_reg(op->simple_dp_reg.source2_reg);
    shift &= 0xff;

    result = ASR(a, shift);
    if (shift == 0) {
        carry = get_condition(PSR_CC_CARRY);
    } else if (shift < 32) {
        carry = BIT(a, shift - 1);
    } else { // RmRsval >= 32
        carry = BIT(a, 31);
    }

    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// rotate right by register, PC may not be target
void uop_ror_reg(struct uop *op);
void uop_ror_reg(struct uop *op)
{
    dword a;
    dword rotate;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    rotate = get_reg(op->simple_dp_reg.source2_reg);
    rotate = BITS(rotate, 4, 0);

    result = ROR(a, rotate);

    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// rotate right by register, S bit, PC may not be target
void uop_ror_reg_s(struct uop *op);
void uop_ror_reg_s(struct uop *op)
{
    int carry;
    dword a;
    dword rotate;
    dword rotate_lower_4_bits;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    rotate = get_reg(op->simple_dp_reg.source2_reg);
    rotate_lower_4_bits = BITS(rotate, 4, 0);

    result = ROR(a, rotate_lower_4_bits);
    if (BITS(rotate, 7, 0) == 0) {
        carry = get_condition(PSR_CC_CARRY);
    } else if (rotate_lower_4_bits == 0) {
        carry = BIT(a, 31);
    } else {
        carry = BIT(a, rotate_lower_4_bits - 1);
    }

    set_NZ_condition(result);
    set_condition(PSR_CC_CARRY, carry);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

/*================================================================*/
// and by register, S bit, PC may not be target
void uop_and_reg_s(struct uop *op);
void uop_and_reg_s(struct uop *op)
{
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = a & b;

    set_NZ_condition(result);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// xor by register, S bit, PC may not be target
void uop_eor_reg_s(struct uop *op);
void uop_eor_reg_s(struct uop *op)
{
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = a ^ b;

    set_NZ_condition(result);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

/*================================================================*/
// bit clear by register, S bit, PC may not be target
void uop_bic_reg_s(struct uop *op);
void uop_bic_reg_s(struct uop *op)
{
    dword a;
    dword b;
    dword result;

    a = get_reg(op->simple_dp_reg.source_reg);
    b = get_reg(op->simple_dp_reg.source2_reg);
    result = a & ~b;

    set_NZ_condition(result);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// negate register, S bit, PC may not be target
void uop_neg_reg_s(struct uop *op);
void uop_neg_reg_s(struct uop *op)
{
    int carry, ovl;
    dword b;
    dword result;

    b = get_reg(op->simple_dp_reg.source2_reg);
    result = do_add(0, ~b, 1, &carry, &ovl);

    set_condition(PSR_CC_CARRY, carry);
    set_condition(PSR_CC_OVL, ovl);
    set_NZ_condition(result);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

// bitwise negation register, S bit, PC may not be target
void uop_mvn_reg_s(struct uop *op);
void uop_mvn_reg_s(struct uop *op)
{
    dword b;
    dword result;

    b = get_reg(op->simple_dp_reg.source2_reg);
    result = ~b;

    set_NZ_condition(result);
    put_reg_nopc(op->simple_dp_reg.dest_reg, result);

    pfm_cnt_op_data(op);
}

/*================================================================*/
void uop_multiply(struct uop *op);
void uop_multiply(struct uop *op)
{
    // multiply the first two operands
    dword temp_dword = get_reg(op->mul.source_reg);
    dword temp_dword2 = get_reg(op->mul.source2_reg) * temp_dword;

    // add a third one conditionally
    if (op->flags & UOPMULFLAGS_ACCUMULATE)
        temp_dword2 += get_reg(op->mul.accum_reg);

    // store the result
    put_reg(op->mul.dest_reg, temp_dword2);

    // set the NZ bits on exit
    if (op->flags & UOPMULFLAGS_S_BIT)
        set_NZ_condition(temp_dword2);

    pfm_cyc_op_mul(op);
    pfm_cnt_op_mul(op);
}

void uop_multiply_long(struct uop *op);
void uop_multiply_long(struct uop *op)
{
    dword reslo, reshi;
    uint64_t result;

    // get the first two operands
    dword temp_dword = get_reg(op->mull.source_reg);
    dword temp_dword2 = get_reg(op->mull.source2_reg);

    // signed or unsigned multiply
    // XXX is this correct
    if (op->flags & UOPMULFLAGS_SIGNED) {
        result = (int64_t)(int)temp_dword * (int)temp_dword2;
    } else {
        result = (uint64_t)temp_dword * temp_dword2;
    }

    // accumulate
    if (op->flags & UOPMULFLAGS_ACCUMULATE) {
        uint64_t acc = get_reg(op->mull.desthi_reg);
        acc = (acc << 32) | get_reg(op->mull.destlo_reg);
        result += acc;
    }

    // store the results
    reslo = result;
    reshi = result >> 32;
    put_reg(op->mull.destlo_reg, reslo);
    put_reg(op->mull.desthi_reg, reshi);

    // set the NZ bits on exit
    if (op->flags & UOPMULFLAGS_S_BIT) {
        set_condition(PSR_CC_NEG, BIT(reshi, 31));
        set_condition(PSR_CC_ZERO, (reslo | reshi) == 0);
    }

    pfm_cyc_op_mul_long(op);
    pfm_cnt_op_mul(op);
}

/*================================================================*/
void uop_swap(struct uop *op);
void uop_swap(struct uop *op)
{
    dword mem_reg_val, source_reg_val;
    armaddr_t addr;

    ASSERT_VALID_REG(op->swp.dest_reg);
    ASSERT_VALID_REG(op->swp.source_reg);
    ASSERT_VALID_REG(op->swp.mem_reg);

    source_reg_val = get_reg(op->swp.source_reg);
    mem_reg_val = get_reg(op->swp.mem_reg);

    addr = mem_reg_val & 0xfffffffc;

    if (!op->swp.b) {
        dword temp;
        if (mmu_read_mem_dword(addr, &temp))
            return; // data abort

        // simulate the weird unaligned access behavior
        switch (mem_reg_val & 0x3) {
            default:
            case 0:
                break;
            case 1:
                temp = ROR(temp, 8);
                break;
            case 2:
                temp = ROR(temp, 16);
                break;
            case 3:
                temp = ROR(temp, 24);
                break;
        }

        // do the swap
        if (mmu_write_mem_dword(addr, source_reg_val))
            return; // data abort
        put_reg(op->swp.dest_reg, temp);
    } else {
        // byte version
        byte temp;
        if (mmu_read_mem_byte(addr, &temp))
            return; // data abort
        if (mmu_write_mem_byte(addr, source_reg_val))
            return; // data abort
        put_reg(op->swp.dest_reg, temp);
    }
}

/*================================================================*/
void uop_count_leading_zeros(struct uop *op);
void uop_count_leading_zeros(struct uop *op)
{
    dword val;
    int count;

    ASSERT_VALID_REG(op->count_leading_zeros.source_reg);
    ASSERT_VALID_REG(op->count_leading_zeros.dest_reg);

    // get the value we're supposed to count
    val = get_reg(op->count_leading_zeros.source_reg);

    count = clz(val);

    // put the result back
    put_reg(op->count_leading_zeros.dest_reg, count);

    // XXX cycle count, or is it always 1 cycle?

    pfm_cnt_op_misc(op);
}

/*================================================================*/
void uop_move_to_sr_imm(struct uop *op);
void uop_move_to_sr_imm(struct uop *op)
{
    reg_t old_psr, new_psr;

    if (op->flags & UOPMSR_R_BIT) {
        // spsr
        old_psr = cpu.spsr;
    } else {
        // cpsr
        old_psr = cpu.cpsr;
    }

    dword field_mask = op->move_to_sr_imm.field_mask;

    // if we're in user mode, we can only modify the top 8 bits
    if (!arm_in_priviledged())
        field_mask &= 0xff000000;

    // or in the new immediate value
    new_psr = (old_psr & ~field_mask) | (op->move_to_sr_imm.immediate & field_mask);

    // write the new value back
    if (op->flags & UOPMSR_R_BIT) {
        // spsr
        // NOTE: UNPREDICTABLE if the cpu is in user or system mode
        cpu.spsr = new_psr;
    } else {
        // cpsr
        set_cpu_mode(new_psr & PSR_MODE_MASK);
        cpu.cpsr = new_psr;

        pfm_cyc_op_mov_to_sr_imm(op);
    }

    pfm_cnt_op_misc(op);
}

void uop_move_to_sr_reg(struct uop *op);
void uop_move_to_sr_reg(struct uop *op)
{
    reg_t old_psr, new_psr;

    if (op->flags & UOPMSR_R_BIT) {
        // spsr
        old_psr = cpu.spsr;
    } else {
        // cpsr
        old_psr = cpu.cpsr;
    }

    dword field_mask = op->move_to_sr_imm.field_mask;
    dword temp_dword = get_reg(op->move_to_sr_reg.reg);

    // if we're in user mode, we can only modify the top 8 bits
    if (!arm_in_priviledged())
        field_mask &= 0xff000000;

    // or in the new immediate value
    new_psr = (old_psr & ~field_mask) | (temp_dword & field_mask);

    // write the new value back
    if (op->flags & UOPMSR_R_BIT) {
        // spsr
        // NOTE: UNPREDICTABLE if the cpu is in user or system mode
        cpu.spsr = new_psr;
    } else {
        // cpsr
        set_cpu_mode(new_psr & PSR_MODE_MASK);
        cpu.cpsr = new_psr;
        pfm_cyc_op_mov_to_sr_reg(op);
    }

    pfm_cnt_op_misc(op);
}

void uop_move_from_sr(struct uop *op);
void uop_move_from_sr(struct uop *op)
{
    if (op->flags & UOPMSR_R_BIT) {
        // NOTE: UNPREDICTABLE if the cpu is in user or system mode
        put_reg(op->move_from_sr.reg, cpu.spsr);
    } else {
        put_reg(op->move_from_sr.reg, cpu.cpsr);
    }

    pfm_cnt_op_misc(op);

    pfm_cyc_op_mov_from_sr(op);
}

/*================================================================*/
void uop_undefined(struct uop *op);
void uop_undefined(struct uop *op)
{
    atomic_or(&cpu.pending_exceptions, EX_UNDEFINED);

//  UOP_TRACE(0, "undefined instruction at 0x%x\n", get_reg(PC));

    pfm_cnt_op_misc(op);
    pfm_cyc_op_und(op);
}
/*================================================================*/
void uop_swi(struct uop *op);
void uop_swi(struct uop *op)
{
    atomic_or(&cpu.pending_exceptions, EX_SWI);

    // always takes 3 cycles
    pfm_cnt_op_misc(op);
    pfm_cyc_op_swi(op);
}

void uop_bkpt(struct uop *op);
void uop_bkpt(struct uop *op)
{
    atomic_or(&cpu.pending_exceptions, EX_PREFETCH);

    // always takes 3 cycles
    pfm_cnt_op_misc(op);
    pfm_cyc_op_bkpt(op);
}


/*================================================================*/

/* end of cpu_ops.c */
