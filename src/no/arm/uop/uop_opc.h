
/*------------------------------------
 * uop_opc.h
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

#ifndef NO_ARM_UOP_UOP_OPC_H_
#define NO_ARM_UOP_UOP_OPC_H_
/*================================================================*/

/* opcodes supported by the uop interpreter */
enum uop_opcode {
    DECODE_ME_ARM = 0,
    DECODE_ME_THUMB,
    NOP,
    B_IMMEDIATE,
    B_IMMEDIATE_LOCAL,          // immediate branch, local to this codepage, has to be in the same mode (ARM or THUMB)
    B_REG,
    B_REG_OFFSET,               // add an immediate offset to a register (second half of thumb bl/blx)
    LOAD_IMMEDIATE_DWORD,       // fast forms for pc relative loads
    LOAD_IMMEDIATE_WORD,    	// "
    LOAD_IMMEDIATE_BYTE,        // "
    LOAD_IMMEDIATE_OFFSET,      // add an immediate to base register
    LOAD_SCALED_REG_OFFSET,     // add base register to shifted second register
    STORE_IMMEDIATE_OFFSET,     // add an immediate to base register
    STORE_SCALED_REG_OFFSET,    // add base register to shifted second register

    LOAD_MULTIPLE,              // simple multiple load, no S bit
    LOAD_MULTIPLE_S,
    STORE_MULTIPLE,             // simple multiple store, no S bit
    STORE_MULTIPLE_S,

    // data processing, ARM style
    DATA_PROCESSING_IMM,        // plain instruction, no barrel shifter, no condition flag update, immediate operand
    DATA_PROCESSING_REG,        // plain instruction, no barrel shifter, no condition flag update, register operand
    DATA_PROCESSING_IMM_S,      // S bit set, update condition flags
    DATA_PROCESSING_REG_S,      // S bit set, update condition flags
    DATA_PROCESSING_IMM_SHIFT,  // barrel shifter involved, immediate operands to shifter, S bit may be involved
    DATA_PROCESSING_REG_SHIFT,  // barrel shifter involved, register operands to shifter, S bit may be involved

    // special versions of some of the above instructions
    MOV_IMM,                    // mov and mvn
    MOV_IMM_NZ,                 // mov and mvn, sets NZ condition bits
    MOV_REG,                    // mov
    CMP_IMM_S,                  // cmp and cmn, sets full conditions
    CMP_REG_S,                  // cmp, sets full conditions
    CMN_REG_S,                  // cmn, sets full conditions
    TST_REG_S,                  // cmn, sets full conditions
    ADD_IMM,                    // add and sub
    ADD_IMM_S,                  // add and sub, sets full conditions
    ADD_REG,                    // add
    ADD_REG_S,                  // add, sets full conditions
    ADC_REG_S,                  // add with carry, sets full conditions
    SUB_REG_S,                  // subtract, sets full conditions
    SBC_REG_S,                  // subtract with carry, sets full conditions

    AND_IMM,                    // and with immediate value, no conditions
    ORR_IMM,                    // orr with immediate value, no conditions
    ORR_REG_S,                  // orr, sets NZ condition bits

    LSL_IMM,                    // logical left shift by immediate
    LSL_IMM_S,                  // logical left shift by immediate, sets full conditions
    LSL_REG,                    // logical left shift by register
    LSL_REG_S,                  // logical left shift by register, sets full conditions
    LSR_IMM,                    // logical right shift by immediate
    LSR_IMM_S,                  // logical right shift by immediate, sets full conditions
    LSR_REG,                    // logical right shift by register
    LSR_REG_S,                  // logical right shift by register, sets full conditions
    ASR_IMM,                    // arithmetic right shift by immediate
    ASR_IMM_S,                  // arithmetic right shift by immediate, sets full conditions
    ASR_REG,                    // arithmetic right shift by register
    ASR_REG_S,                  // arithmetic right shift by register, sets full conditions
    ROR_REG,                    // rotate right by register
    ROR_REG_S,                  // rotate right by register, sets full conditions

    AND_REG_S,                  // and, sets NZ condition bits
    EOR_REG_S,                  // exclusive or, sets NZ condition bits
    BIC_REG_S,                  // and with complement, sets NZ condition bits
    NEG_REG_S,                  // subtract from 0, sets full condition bits
    MVN_REG_S,                  // bitwise reverse, sets NZ condition bits

    // multiply variants
    MULTIPLY,
    MULTIPLY_LONG,

    SWAP,

    // count leading zeros
    COUNT_LEADING_ZEROS,

    // move from/to status register
    MOVE_TO_SR_IMM,
    MOVE_TO_SR_REG,
    MOVE_FROM_SR,

    // various exceptions
    UNDEFINED,
    SWI,
    BKPT,

    // coprocessor
    COPROC_REG_TRANSFER,
    COPROC_DOUBLE_REG_TRANSFER,
    COPROC_DATA_PROCESSING,
    COPROC_LOAD_STORE,

    MAX_UOP_OPCODE,
};

/*================================================================*/
/* arm arithmetic opcodes */
enum {
    AOP_AND = 0,
    AOP_EOR,
    AOP_SUB,
    AOP_RSB,
    AOP_ADD,
    AOP_ADC,
    AOP_SBC,
    AOP_RSC,
    AOP_TST,
    AOP_TEQ,
    AOP_CMP,
    AOP_CMN,
    AOP_ORR,
    AOP_MOV,
    AOP_BIC,
    AOP_MVN,
};

/*================================================================*/
#endif 
/* end of uop_opc.h */
