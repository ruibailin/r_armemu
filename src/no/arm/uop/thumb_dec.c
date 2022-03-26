
/*------------------------------------
 * uop_thumb_dec.c
 * Create:  2021-02-11
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
#include "thumb_ops.h"

#include "../cpu/cpu_bits.h"
/*================================================================*/
//for thumb. all ins are word
void uop_from_tmb_decode(struct uop *op);
void uop_from_tmb_decode(struct uop *op)
{
    word ins = op->undecoded.raw_instruction;

    // switch based off the top 5 bits of the instruction
    switch (BITS_SHIFT(ins, 15, 11)) {
        case 0x00:
        case 0x01:
        case 0x02:
            // shift by immediate
            uop_tmb_shift_imm(op);
            break;
        case 0x03:
            // add/subtract 3 register
            uop_tmb_add_sub_3reg(op);
            break;
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            // add/subtract/compare/move immediate
            uop_tmb_add_sub_large_immediate(op);
            break;
        case 0x08:
            if (BIT(ins, 10)) {
                if (BITS_SHIFT(ins, 9, 8) == 3) {
                    // branch/exchange instruction set (also BLX)
                    uop_tmb_bx_reg(op);
                } else {
                    // special data processing
                    uop_tmb_special_data_processing(op);
                }
            } else {
                // data-processing register
                uop_tmb_data_processing(op);
            }
            break;
        case 0x09:
            // load from literal pool
            uop_tmb_literal_load(op);
            break;
        case 0x0a:
        case 0x0b:
            // load/store register offset
            uop_tmb_load_store_register(op);
            break;
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
            // load/store word/byte immediate offset
            uop_tmb_load_store_immediate_offset(op);
            break;
        case 0x10:
        case 0x11:
            // load/store word immediate offset
            uop_tmb_load_store_immediate_offset(op);
            break;
        case 0x12:
        case 0x13:
            // load/store to/from stack
            uop_tmb_load_store_from_stack(op);
            break;
        case 0x14:
        case 0x15:
            // add to sp or pc
            uop_tmb_add_to_sp_pc(op);
            break;
        case 0x16:
        case 0x17:
            // miscellaneous (figure 6-2, page A6-5)
            switch (BITS_SHIFT(ins, 11, 8)) {
                case 0:
                    // adjust stack pointer
                    uop_tmb_adjust_sp(op);
                    break;
                case 0x4:
                case 0x5:
                case 0xc:
                case 0xd:
                    // push/pop register list
                    uop_tmb_push_pop_regs(op);
                    break;
                case 0xe:
                    // software breakpoint
                    uop_tmb_software_breakpoint(op);
                    break;
                default:
                    // undefined instruction
                    uop_tmb_undefined(op);
                    break;
            }
            break;
        case 0x18:
        case 0x19:
            // load store multiple
            uop_tmb_load_store_multiple(op);
            break;
        case 0x1a:
        case 0x1b:
            switch (BITS_SHIFT(ins, 11, 8)) {
                default:
                    // conditional branch
                    uop_tmb_conditional_branch(op);
                    break;
                case 0xe:
                    // undefined instruction
                    uop_tmb_undefined(op);
                    break;
                case 0xf:
                    // software interrupt
                    uop_tmb_swi(op);
                    break;
            }
            break;
        case 0x1c:
            // unconditional branch
            uop_tmb_branch(op);
            break;
        case 0x1d:
            if (ins & 1) {
                // undefined instruction
                uop_tmb_undefined(op);
            } else {
                // blx suffix
                uop_tmb_bl_blx(op);
            }
            break;
        case 0x1e:
        case 0x1f:
            // bl/blx prefix
            uop_tmb_bl_blx(op);
            break;
    }
}

/*================================================================*/
void thumb_decode_into_uop(struct uop *op);
void thumb_decode_into_uop(struct uop *op)
{
	uop_from_tmb_decode(op);

}

/*================================================================*/
/* end of uop_thumb_dec.c */
