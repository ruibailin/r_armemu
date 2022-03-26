
/*------------------------------------
 * cpu_dec.c
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
#include "cpu_ops.h"
#include "cop_ops.h"

#include "../pfm/pfm.h"
#include "../cdt/cdt.h"
#include "../cdp/cdp.h"
#include "../cpu/cpu.h"
#include "../mmu/mmu.h"
/*================================================================*/
void uop_dump_cpu(struct uop *op);
void uop_dump_cpu(struct uop *op)
{
#if DYNAMIC_TRACE_LEVELS
        if (TRACE_CPU_LEVEL >= 10
                && op->opcode != DECODE_ME_ARM
                && op->opcode != DECODE_ME_THUMB)
            dump_cpu();
#endif
}

static void print_ins(struct uop *op)
{
	if(op->opcode == DECODE_ME_ARM)
		return;
	if(op->opcode == DECODE_ME_THUMB)
		return;
	os_printf("%x:%x   %s\n",op->add,op->raw,uop_opc_to_str(op->opcode));
}

/*================================================================*/
struct uop *uop_from_cpu_decode(struct uop *op);
struct uop *uop_from_cpu_decode(struct uop *op)
{
    UOP_TRACE(10, "\nUOP: start of new cycle\n");

    // in the last instruction we wrote something else into r[PC], so sync it with
    // the real program counter cpu.pc
    if (unlikely(cpu.r15_dirty)) {
        UOP_TRACE(9, "UOP: r15 dirty\n");
        cpu.r15_dirty = FALSE;

        if (cdp.curr_cp) {
            if ((cpu.pc >> MMU_PAGESIZE_SHIFT) == (cpu.r[PC] >> MMU_PAGESIZE_SHIFT)) {
                cpu.cp_pc = PC_TO_CPPC(cpu.r[PC]);
            } else {
                cdp.curr_cp = NULL; // will load a new codepage in a few lines
            }
        }
        cpu.pc = cpu.r[PC];
    }

    // check for exceptions
    if (unlikely(cpu.pending_exceptions != 0)) {
        // something may be pending
        if (cpu.pending_exceptions & ~(cpu.cpsr & (PSR_IRQ_MASK|PSR_FIQ_MASK))) {
            if (process_pending_exceptions())
                return op;
        }
    }

    /* see if we are off the end of a codepage, or the codepage was removed out from underneath us */
    if (unlikely(cdp.curr_cp == NULL)) {
        UOP_TRACE(7, "UOP: curr_cp == NULL, setting new codepage\n");
        if (set_codepage(cpu.pc))
            return op; // MMU translation error reading it
    }

    ASSERT(cdp.curr_cp != NULL);
    ASSERT(cpu.cp_pc != NULL);

    // instruction count
    pfm_cnt_ins();
    pfm_cyc_inc();

    /* get the next op and dispatch it */
    op = cpu.cp_pc;
    UOP_TRACE(8, "UOP: opcode %3d %32s, pc 0x%x, cp_pc %p, curr_cp %p\n", op->opcode, uop_opcode_to_str(op->opcode), cpu.pc, cpu.cp_pc, cdp.curr_cp);
    pfm_cnt_uops(op);

    /* increment the program counter */
    int pc_inc = cdp.curr_cp->pc_inc;
    cpu.pc += pc_inc; // next pc
    cpu.r[PC] = cpu.pc + pc_inc; // during the course of the instruction, r15 looks like it's +8 or +4 (arm vs thumb)
    cpu.cp_pc++;

    cpu_trace_dump();

    /* check to see if we should execute it */
    if (unlikely(!cdt_check_table(op->cond))) {
        UOP_TRACE(8, "UOP: opcode not executed due to condition 0x%x\n", op->cond);
        pfm_cnt_op_skip(op);
        return op; // not executed
    }

//    print_ins(op);

    /* dispatch */
    switch (op->opcode) {
        case NOP:
            pfm_cnt_op_nop(op);
            break;
        case DECODE_ME_ARM:
        	op->add=cpu.pc - pc_inc;
        	op->raw=op->undecoded.raw_instruction;
            uop_decode_me_arm(op);
            break;
        case DECODE_ME_THUMB:
        	op->add=cpu.pc - pc_inc;
        	op->raw=op->undecoded.raw_instruction;
            uop_decode_me_thumb(op);
            break;
        case B_IMMEDIATE:
            uop_b_immediate(op);
            break;
        case B_IMMEDIATE_LOCAL:
            uop_b_immediate_local(op);
            break;
        case B_REG:
            uop_b_reg(op);
            break;
        case B_REG_OFFSET:
            uop_b_reg_offset(op);
            break;
        case LOAD_IMMEDIATE_DWORD:
            uop_load_immediate_dword(op);
            break;
        case LOAD_IMMEDIATE_WORD:
            uop_load_immediate_word(op);
            break;
        case LOAD_IMMEDIATE_BYTE:
            uop_load_immediate_byte(op);
            break;
        case LOAD_IMMEDIATE_OFFSET:
            uop_load_immediate_offset(op);
            break;
        case LOAD_SCALED_REG_OFFSET:
            uop_load_scaled_reg_offset(op);
            break;
        case STORE_IMMEDIATE_OFFSET:
            uop_store_immediate_offset(op);
            break;
        case STORE_SCALED_REG_OFFSET:
            uop_store_scaled_reg_offset(op);
            break;
        case LOAD_MULTIPLE:         // simple multiple load, no S bit
            uop_load_multiple(op);
            break;
        case LOAD_MULTIPLE_S:               // multiple load, with S bit
            uop_load_multiple_s(op);
            break;
        case STORE_MULTIPLE:            // simple multiple store, no S bit
            uop_store_multiple(op);
            break;
        case STORE_MULTIPLE_S:      // multiple store with S bit
            uop_store_multiple_s(op);
            break;
        case DATA_PROCESSING_IMM:       // plain instruction, no barrel shifter, no condition flag update, immediate operand
            uop_data_processing_imm(op);
            break;
        case DATA_PROCESSING_REG:       // plain instruction, no barrel shifter, no condition flag update, no writeback, register operand
            uop_data_processing_reg(op);
            break;
        case DATA_PROCESSING_IMM_S: // same as above but S bit set, update condition flags, writeback conditional on op
            uop_data_processing_imm_s(op);
            break;
        case DATA_PROCESSING_REG_S: // same as above but S bit set, update condition flags, writeback conditional on op
            uop_data_processing_reg_s(op);
            break;
        case DATA_PROCESSING_IMM_SHIFT: // barrel shifter involved, immediate operands to shifter, S bit may apply
            uop_data_processing_imm_shift(op);
            break;
        case DATA_PROCESSING_REG_SHIFT: // barrel shifter involved, register operands to shifter, S bit may apply
            uop_data_processing_reg_shift(op);
            break;
        case MOV_IMM: // move immediate value into register
            uop_mov_imm(op);
            break;
        case MOV_IMM_NZ: // move immediate value into register, set NZ condition
            uop_mov_imm_nz(op);
            break;
        case MOV_REG: // move one register to another
            uop_mov_reg(op);
            break;
        case CMP_IMM_S: // compare with immediate, set full conditions
            uop_cmp_imm_s(op);
            break;
        case CMP_REG_S: // compare with another register, set full conditions
            uop_cmp_reg_s(op);
            break;
        case CMN_REG_S:
            uop_cmn_reg_s(op);
            break;
        case TST_REG_S:
            uop_tst_reg_s(op);
            break;
        case ADD_IMM: // add a value to a register and store it into another register
            uop_add_imm(op);
            break;
        case ADD_IMM_S: // add a value to a register and store it into another register, set condition flags
            uop_add_imm_s(op);
            break;
        case ADD_REG: // add two registers and store in a third
            uop_add_reg(op);
            break;
        case ADD_REG_S: // add two registers and store in a third, set condition flags
            uop_add_reg_s(op);
            break;
        case ADC_REG_S:
            uop_adc_reg_s(op);
            break;
        case SUB_REG_S: // add two registers and store in a third, set condition flags
            uop_sub_reg_s(op);
            break;
        case SBC_REG_S:
            uop_sbc_reg_s(op);
            break;
        case AND_IMM:
            uop_and_imm(op);
            break;
        case ORR_IMM:
            uop_orr_imm(op);
            break;
        case ORR_REG_S:
            uop_orr_reg_s(op);
            break;
        case LSL_IMM:   // logical left shift by immediate
            uop_lsl_imm(op);
            break;
        case LSL_IMM_S: // logical left shift by immediate, sets full conditions
            uop_lsl_imm_s(op);
            break;
        case LSL_REG:   // logical left shift by register
            uop_lsl_reg(op);
            break;
        case LSL_REG_S: // logical left shift by register, sets full conditions
            uop_lsl_reg_s(op);
            break;
        case LSR_IMM:   // logical right shift by immediate
            uop_lsr_imm(op);
            break;
        case LSR_IMM_S: // logical right shift by immediate, sets full conditions
            uop_lsr_imm_s(op);
            break;
        case LSR_REG:   // logical right shift by register
            uop_lsr_reg(op);
            break;
        case LSR_REG_S: // logical right shift by register, sets full conditions
            uop_lsr_reg_s(op);
            break;
        case ASR_IMM:   // arithmetic right shift by immediate
            uop_asr_imm(op);
            break;
        case ASR_IMM_S: // arithmetic right shift by immediate, sets full conditions
            uop_asr_imm_s(op);
            break;
        case ASR_REG:   // arithmetic right shift by register
            uop_asr_reg(op);
            break;
        case ASR_REG_S: // arithmetic right shift by register, sets full conditions
            uop_asr_reg_s(op);
            break;
        case ROR_REG:   // rotate right by register
            uop_ror_reg(op);
            break;
        case ROR_REG_S: // rotate right by register, sets full conditions
            uop_ror_reg_s(op);
            break;
        case AND_REG_S:
            uop_and_reg_s(op);
            break;
        case EOR_REG_S:
            uop_eor_reg_s(op);
            break;
        case BIC_REG_S:
            uop_bic_reg_s(op);
            break;
        case NEG_REG_S:
            uop_neg_reg_s(op);
            break;
        case MVN_REG_S:
            uop_mvn_reg_s(op);
            break;
        case MULTIPLY:
            uop_multiply(op);
            break;
        case MULTIPLY_LONG:
            uop_multiply_long(op);
            break;
        case SWAP:
            uop_swap(op);
            break;
        case COUNT_LEADING_ZEROS:
            uop_count_leading_zeros(op);
            break;
        case MOVE_TO_SR_IMM:
            uop_move_to_sr_imm(op);
            break;
        case MOVE_TO_SR_REG:
            uop_move_to_sr_reg(op);
            break;
        case MOVE_FROM_SR:
            uop_move_from_sr(op);
            break;
        case UNDEFINED:
            uop_undefined(op);
            break;
        case SWI:
            uop_swi(op);
            break;
        case BKPT:
            uop_bkpt(op);
            break;
        case COPROC_REG_TRANSFER:
            uop_cop_reg_transfer(op);
            break;
        case COPROC_DOUBLE_REG_TRANSFER:
            uop_cop_double_reg_transfer(op);
            break;
        case COPROC_DATA_PROCESSING:
            uop_cop_data_processing(op);
            break;
        case COPROC_LOAD_STORE:
            uop_cop_load_store(op);
            break;
        default:
            panic_cpu("bad uop decode, bailing...\n");
    }
    return op;
}

/*================================================================*/

/* end of cpu_dec.c */
