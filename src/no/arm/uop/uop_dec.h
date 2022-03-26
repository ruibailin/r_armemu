
/*------------------------------------
 * uop_dec.h
 * Create:  2021-02-10
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

#ifndef NO_ARM_UOP_DEC_H_
#define NO_ARM_UOP_DEC_H_
/*================================================================*/
#include "../1imp.h"

/* description for the internal opcode format and decoder routines */
struct uop {
    word opcode;
    byte cond; // 4 bits of condition
    byte flags; // up to 8 flags
    dword add;  // record address of instruction,added by rbl
    dword raw;	//keep undecoded instruction, added by rbl
    union {
        struct {
            // undecoded, arm or thumb
            dword raw_instruction;
        } undecoded;

        // branch opcodes
#define UOPBFLAGS_LINK                  0x1
#define UOPBFLAGS_SETTHUMB_ALWAYS       0x2
#define UOPBFLAGS_UNSETTHUMB_ALWAYS     0x4
#define UOPBFLAGS_SETTHUMB_COND         0x8 // set the thumb bit conditionally on the new value loaded
        struct {
            dword target;
            dword link_target;
            struct uop_codepage *target_cp; // once it's executed, cache a copy of the target codepage, if it's a nonlocal jump
        } b_immediate;
        struct {
            dword reg;
            dword link_offset;
        } b_reg;
        struct {
            dword reg;
            dword link_offset;
            dword offset;
        } b_reg_offset;

        // load/store opcodes
#define UOPLSFLAGS_WRITEBACK        0x1 // write the computed address back into the source register, set on postindex ops
#define UOPLSFLAGS_NEGATE_OFFSET    0x2 // negate the offset before applying it
#define UOPLSFLAGS_SIGN_EXTEND  0x4
#define UOPLSFLAGS_POSTINDEX        0x8
#define UOPLSFLAGS_SIZE_LONG       	(0<<4)
#define UOPLSFLAGS_SIZE_DWORD       (1<<4)
#define UOPLSFLAGS_SIZE_WORD    	(2<<4)
#define UOPLSFLAGS_SIZE_BYTE        (3<<4)
#define UOPLSFLAGS_SIZE_MASK        (3<<4)
        struct {
            dword address;       // address is precomputed (pc relative loads)
            byte target_reg;
        } load_immediate;
        struct {
            dword offset;        // offset is precomputed
            byte target_reg;
            byte source_reg;
        } load_store_immediate_offset;
        struct {
            dword shift_immediate;
            byte target_reg;
            byte source_reg;
            byte source2_reg;
            byte shift_op; // LSL, LSR, ASR, ROR
        } load_store_scaled_reg_offset;

        // load/store multiple
#define UOPLSMFLAGS_WRITEBACK               0x1 // update the base reg with the writeback address
#define UOPLSMFLAGS_LOAD_CPSR               0x2 // in one form of load multiple, if the S bit is set and the PC is in the list, do a normal load + load cpsr from spsr
        struct {
            int16_t  base_offset;       // add to base reg to get starting address (may be negative)
            int16_t  writeback_offset;  // add to base reg to get writeback address (may be negative)
            word 	 reg_bitmap;
            byte     reg_count;
            byte     base_reg;
        } load_store_multiple;

        // data processing opcodes
#define UOPDPFLAGS_SET_CARRY_FROM_SHIFTER   0x1 // use the following bit to set the carry bit if writeback
#define UOPDPFLAGS_CARRY_FROM_SHIFTER       0x2 // carry out of the barrel shifter, in case it was factored out and store as an immediate
#define UOPDPFLAGS_S_BIT                    0x4 // in the case of the barrel shifter instructions, conditional writeback is optional based on the S bit
        struct {
            byte dp_opcode; // and, or, add, etc
            byte dest_reg;
            byte source_reg;
            dword immediate;
        } data_processing_imm;
        struct {
            byte dp_opcode; // and, or, add, etc
            byte dest_reg;
            byte source_reg;
            byte source2_reg;
        } data_processing_reg;
        struct {
            byte dp_opcode; // and, or, add, etc
            byte dest_reg;
            byte source_reg;
            byte source2_reg;
            byte shift_opcode;
            byte shift_imm;
        } data_processing_imm_shift;
        struct {
            byte dp_opcode; // and, or, add, etc
            byte dest_reg;
            byte source_reg;
            byte source2_reg;
            byte shift_opcode;
            byte shift_reg;
        } data_processing_reg_shift;

        // special versions of some of the above instructions
        struct {
            dword immediate;
            byte dest_reg;
            byte source_reg;
        } simple_dp_imm;
        struct {
            byte dest_reg;
            byte source_reg;
            byte source2_reg;
        } simple_dp_reg;

        // multiply
#define UOPMULFLAGS_S_BIT                   0x1 // set NZ condition
#define UOPMULFLAGS_ACCUMULATE              0x2 // add the contents of the accum reg to the product
#define UOPMULFLAGS_SIGNED                  0x4 // signed long multiply (only defined on long multiples)
        struct {
            byte dest_reg;
            byte source_reg;
            byte source2_reg;
            byte accum_reg;
        } mul;
        struct {
            byte destlo_reg;
            byte desthi_reg;
            byte source_reg;
            byte source2_reg;
        } mull;

        struct {
            byte dest_reg;
            byte source_reg;
            byte mem_reg;
            byte b;
        } swp;

        // count leading zeros
        struct {
            byte dest_reg;
            byte source_reg;
        } count_leading_zeros;

        // move from/to status register
#define UOPMSR_R_BIT                        0x1 // access to spsr instead of cpsr
        struct {
            dword immediate;
            dword field_mask;
        } move_to_sr_imm;
        struct {
            byte reg;
            dword field_mask;
        } move_to_sr_reg;
        struct {
            byte reg;
        } move_from_sr;

        // coprocessor instructions
        struct {
            dword raw_instruction;           // the coprocessor parses the instruction
            byte cp_num;
        } coproc;

        struct sizing {
            // space it out to 16 bytes
            unsigned int data0;
            unsigned int data1;
            unsigned int data2;
        } _sizing;
    };
};


/*================================================================*/
#endif 
/* end of uop_dec.h */
