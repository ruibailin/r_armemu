
/*------------------------------------
 * arm_dec.c
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
#include "arm_ops.h"

#include "../cpu/cpu.h"
/*================================================================*/
static void bad_decode(struct uop *op);
static void bad_decode(struct uop *op)
{
    panic_cpu("bad_decode: ins 0x%08x at 0x%08x\n", op->undecoded.raw_instruction, cpu.pc - 4);
}

/*================================================================*/
static void group_0_s1_c2(struct uop *op);
static void group_0_s1_c2(struct uop *op)
{
	dword ins;
	ins=op->undecoded.raw_instruction;
	switch (ins & (3<<5)) { // bits 5 and 6
        case 0: // multiply, multiply long, swap
            switch (ins & (3<<23)) {
                case 0:
                    uop_arm_mul(op); // mul/mla
                    break;
                case (1<<23):
                    uop_arm_mull(op); // umull/smull/umlal/smlal
                    break;
                case (2<<23):
                    uop_arm_swap(op); // swp/swpb
                    break;
                default:
                    uop_arm_undefined(op);
            }
            break;
        default: // load/store halfward and load/store two words
            uop_arm_misc_load_store(op); // load store halfword/doubleword
            break;
    }
}

static void group_0_s1_c1(struct uop *op);
static void group_0_s1_c1(struct uop *op)
{
	dword ins;
	ins=op->undecoded.raw_instruction;

    // also section 3.13.3, page A3-30 (control instruction extension space)
    unsigned int op1 = BITS_SHIFT(ins, 22, 21);

    // switch based off of bits 7:4
    switch (ins & (0x7 << 4)) {
        case (0<<4):
            if (op1 & 1)
                uop_arm_msr(op); // msr
            else
                uop_arm_mrs(op); // mrs
            break;
        case (1<<4):
            switch (op1) {
                case 1:
                    uop_arm_bx(op); // bx
                    break;
                case 3:
                    uop_arm_clz(op); // clz
                    break;
                default:
                    uop_arm_undefined(op); // undefined
                    break;
            }
            break;
        case (3<<4):
            if (op1 == 1) {
                uop_arm_bx(op); // blx, register form
            } else {
                uop_arm_undefined(op); // undefined
            }
            break;
        case (5<<4):
            bad_decode(op); // XXX DSP add/subtracts go here (qadd, qsub, qdadd, qdsub)
            break;
        case (7<<4):
            if (op1 == 1) {
                uop_arm_bkpt(op); // bkpt
            } else {
                uop_arm_undefined(op); // undefined
            }
            break;
        default:
            uop_arm_undefined(op);
            break;
    }
}


// opcode[27:25] == 0b000
static void prim_group_0_decode(struct uop *op);
static void prim_group_0_decode(struct uop *op)
{
	dword ins;
	ins=op->undecoded.raw_instruction;
    /* look for special cases (various miscellaneous forms) */
    switch (ins & ((3<<23)|(1<<20)|(1<<7)|(1<<4))) {
        case (2<<23):
        case (2<<23)|(1<<4):  // miscellaneous instructions (Figure 3-3, page A3-4).
        	group_0_s1_c1(op);
            break;
        // cases of bits 24:23 == 2 and bit 20 == 0 and bit 7 == 1 and bit 4 == 0
        case (2<<23)|(1<<7):
            bad_decode(op);  // XXX DSP multiplies go here (smla, smlaw, smulw, smlal, smul)
            break;
        // all cases of bit 7 and 4 being set
        case                 (1<<7)|(1<<4):
        case (1<<23)|        (1<<7)|(1<<4):
        case (2<<23)|        (1<<7)|(1<<4):
        case (3<<23)|        (1<<7)|(1<<4):
        case         (1<<20)|(1<<7)|(1<<4):
        case (1<<23)|(1<<20)|(1<<7)|(1<<4):
        case (2<<23)|(1<<20)|(1<<7)|(1<<4):
        case (3<<23)|(1<<20)|(1<<7)|(1<<4): { // multiplies, extra load/stores (Figure 3-2, page A3-3)
        	group_0_s1_c2(op);
            break;
        default:
        	uop_arm_data_processing(op); // data processing immediate shift and register shift
        	break;
            // cases of bits 24:23 == 2 and bit 20 == 0 and bit 7 == 0
        }
    }
}

/*================================================================*/
// opcode[27:25] == 0b001
static void prim_group_1_decode(struct uop *op);
static void prim_group_1_decode(struct uop *op)
{
	dword ins;
	ins=op->undecoded.raw_instruction;
    /* look for special cases (undefined, move immediate to status reg) */
    switch (BITS_SHIFT(ins, 24, 20)) {
        case 0x12:
        case 0x16: // MSR, immediate form
            uop_arm_msr(op);
            break;
        case 0x10:
        case 0x14: // undefined
            uop_arm_undefined(op);
            break;
        default:
            uop_arm_data_processing(op);
        	break;
    }
}

/*================================================================*/
// opcode[27:25] == 0b011
static void prim_group_3_decode(struct uop *op);
static void prim_group_3_decode(struct uop *op)
{
	dword ins;
	ins=op->undecoded.raw_instruction;
	/* look for a particular undefined form */
    if (ins & (1<<4))
    	uop_arm_undefined(op);
    else
    	uop_arm_load_store(op); // general load/store
}

/*================================================================*/
// opcode[27:25] == 0b110
static void prim_group_6_decode(struct uop *op);
static void prim_group_6_decode(struct uop *op)
{
	dword ins;
	ins=op->undecoded.raw_instruction;
	/* look for the coprocessor instruction extension space */
    if ((BITS(ins, 24, 23) | BIT(ins, 21)) != 0)
    {
        uop_arm_cop_load_store(op);
        return;
    }

    if (BIT(ins, 22))
        uop_arm_cop_double_reg_transfer(op); // mcrr/mrrc, for ARMv5e
    else
    	uop_arm_undefined(op);
}

/*================================================================*/
#include "../cpu/cpu_cond.h"
// opcode[27:25] == 0b111
static void prim_group_7_decode(struct uop *op);
static void prim_group_7_decode(struct uop *op)
{
	dword ins,opc;
	ins=op->undecoded.raw_instruction;
	opc=ins;
	ins=ins>> COND_SHIFT;
	ins=ins& COND_MASK;

    /* undefined */
    if (ins == COND_SPECIAL) {
        uop_arm_undefined(op);
        return;
    }

    /* switch between swi and coprocessor */
    if (BIT(opc, 24)) {
        uop_arm_swi(op);
        return;
    }

    if (BIT(opc, 4))
        uop_arm_cop_reg_transfer(op);
    else
        uop_arm_cop_data_processing(op);
}

/*================================================================*/
typedef void (*decode_stage_func)(struct uop *op);
const decode_stage_func ins_group_table[] = {
    prim_group_0_decode, // data processing, multiply, load/store halfword/two words
    prim_group_1_decode, // data processing immediate and move immediate to status reg
    uop_arm_load_store,       // load/store immediate offset
    prim_group_3_decode, // load/store register offset
    uop_arm_load_store_multiple, // load/store multiple
    uop_arm_branch,  // branch op
    prim_group_6_decode, // coprocessor load/store and double reg transfers
    prim_group_7_decode, // coprocessor data processing/register transfers, swi, undefined instructions
};

static void ins_is_bad(dword opc,struct uop *op);
static void ins_is_bad(dword opc,struct uop *op)
{
    if ((opc & 0xe0) == 0xa0) {
        uop_arm_branch(op); // blx (address form)
        return;
    }
    if ((opc & 0xe0) == 0xc0) {
        // XXX stc2/ldc2
        bad_decode(op);
        return;
    }
    if ((opc & 0xf0) == 0xe0) {
        // XXX cdp2/mcr2/mrc2
        bad_decode(op);
        return;
    }
    if ((opc & 0xd7) == 0x55) {
        uop_arm_pld(op); // pld
        return;
    }
        uop_arm_undefined(op);
        return;
}
/*================================================================*/
void uop_from_arm_decode(struct uop *op);
void uop_from_arm_decode(struct uop *op)
{
    /* look for the unconditional instruction extension space. page A3-34 */
    // XXX are these "always" instructions on V4?
	dword ins,cls,opc;
	ins=op->undecoded.raw_instruction;
	cls=BITS_SHIFT(ins, 27, 25);
	opc=BITS_SHIFT(ins, 27, 20);

	ins=ins>> COND_SHIFT;
	ins=ins& COND_MASK;
    if (ins == COND_SPECIAL)
    	ins_is_bad(opc,op);
    else
        ins_group_table[cls](op);
    return;
}

/*================================================================*/
void arm_decode_into_uop(struct uop *op);
void arm_decode_into_uop(struct uop *op)
{
	uop_from_arm_decode(op);
}
/*================================================================*/

/* end of arm_dec.c */
