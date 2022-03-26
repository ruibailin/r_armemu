
/*------------------------------------
 * uop_opc.c
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
#include "uop_opc.h"
/*================================================================*/
const char *dp_op_to_str(int op);
const char *dp_op_to_str(int op)
{
    switch (op) {
        case AOP_AND:
            return "AND";
        case AOP_EOR:
            return "EOR";
        case AOP_SUB:
            return "SUB";
        case AOP_RSB:
            return "RSB";
        case AOP_ADD:
            return "ADD";
        case AOP_ADC:
            return "ADC";
        case AOP_SBC:
            return "SBC";
        case AOP_RSC:
            return "RSC";
        case AOP_TST:
            return "TST";
        case AOP_TEQ:
            return "TEQ";
        case AOP_CMP:
            return "CMP";
        case AOP_CMN:
            return "CMN";
        case AOP_ORR:
            return "ORR";
        case AOP_MOV:
            return "MOV";
        case AOP_BIC:
            return "BIC";
        case AOP_MVN:
            return "MVN";
        default:
            return "UNK";
    }
}
/*================================================================*/
const char *uop_opcode_to_str(int opcode);
const char *uop_opcode_to_str(int opcode)
{
#define OP_TO_STR(op) case op: return #op

    switch (opcode) {
            OP_TO_STR(NOP);
            OP_TO_STR(DECODE_ME_ARM);
            OP_TO_STR(DECODE_ME_THUMB);
            OP_TO_STR(B_IMMEDIATE);
            OP_TO_STR(B_IMMEDIATE_LOCAL);
            OP_TO_STR(B_REG);
            OP_TO_STR(B_REG_OFFSET);
            OP_TO_STR(LOAD_IMMEDIATE_DWORD);
            OP_TO_STR(LOAD_IMMEDIATE_WORD);
            OP_TO_STR(LOAD_IMMEDIATE_BYTE);
            OP_TO_STR(LOAD_IMMEDIATE_OFFSET);
            OP_TO_STR(LOAD_SCALED_REG_OFFSET);
            OP_TO_STR(STORE_IMMEDIATE_OFFSET);
            OP_TO_STR(STORE_SCALED_REG_OFFSET);
            OP_TO_STR(LOAD_MULTIPLE);
            OP_TO_STR(LOAD_MULTIPLE_S);
            OP_TO_STR(STORE_MULTIPLE);
            OP_TO_STR(STORE_MULTIPLE_S);
            OP_TO_STR(DATA_PROCESSING_IMM);
            OP_TO_STR(DATA_PROCESSING_IMM_S);
            OP_TO_STR(DATA_PROCESSING_REG);
            OP_TO_STR(DATA_PROCESSING_REG_S);
            OP_TO_STR(DATA_PROCESSING_IMM_SHIFT);
            OP_TO_STR(DATA_PROCESSING_REG_SHIFT);
            OP_TO_STR(MOV_IMM);
            OP_TO_STR(MOV_IMM_NZ);
            OP_TO_STR(MOV_REG);
            OP_TO_STR(CMP_IMM_S);
            OP_TO_STR(CMP_REG_S);
            OP_TO_STR(CMN_REG_S);
            OP_TO_STR(TST_REG_S);
            OP_TO_STR(ADD_IMM);
            OP_TO_STR(ADD_IMM_S);
            OP_TO_STR(ADD_REG);
            OP_TO_STR(ADD_REG_S);
            OP_TO_STR(ADC_REG_S);
            OP_TO_STR(SUB_REG_S);
            OP_TO_STR(SBC_REG_S);
            OP_TO_STR(AND_IMM);
            OP_TO_STR(ORR_IMM);
            OP_TO_STR(ORR_REG_S);
            OP_TO_STR(LSL_IMM);
            OP_TO_STR(LSL_IMM_S);
            OP_TO_STR(LSL_REG);
            OP_TO_STR(LSL_REG_S);
            OP_TO_STR(LSR_IMM);
            OP_TO_STR(LSR_IMM_S);
            OP_TO_STR(LSR_REG);
            OP_TO_STR(LSR_REG_S);
            OP_TO_STR(ASR_IMM);
            OP_TO_STR(ASR_IMM_S);
            OP_TO_STR(ASR_REG);
            OP_TO_STR(ASR_REG_S);
            OP_TO_STR(ROR_REG);
            OP_TO_STR(ROR_REG_S);
            OP_TO_STR(AND_REG_S);
            OP_TO_STR(EOR_REG_S);
            OP_TO_STR(BIC_REG_S);
            OP_TO_STR(NEG_REG_S);
            OP_TO_STR(MVN_REG_S);
            OP_TO_STR(MULTIPLY);
            OP_TO_STR(MULTIPLY_LONG);
            OP_TO_STR(COUNT_LEADING_ZEROS);
            OP_TO_STR(MOVE_TO_SR_IMM);
            OP_TO_STR(MOVE_TO_SR_REG);
            OP_TO_STR(MOVE_FROM_SR);
            OP_TO_STR(UNDEFINED);
            OP_TO_STR(SWI);
            OP_TO_STR(BKPT);
            OP_TO_STR(COPROC_REG_TRANSFER);
            OP_TO_STR(COPROC_DOUBLE_REG_TRANSFER);
            OP_TO_STR(COPROC_DATA_PROCESSING);
            OP_TO_STR(COPROC_LOAD_STORE);
        default:
            return "UNKNOWN";
    }
#undef OP_TO_STR
}

/*================================================================*/
static const char *dpc_str[]={
        "AND",
        "EOR",
        "SUB",
        "RSB",
        "ADD",
        "ADC",
        "SBC",
        "RSC",
        "TST",
        "TEQ",
        "CMP",
        "CMN",
        "ORR",
        "MOV",
        "BIC",
        "MVN",
        "UNK",
};

const char *uop_dpc_to_str(int op);
const char *uop_dpc_to_str(int op)
{
	return dpc_str[op];
}

/*================================================================*/
static const char *opc_str[]={
		"DECODE_ME_ARM",
		"DECODE_ME_THUMB",
		"NOP",
		"B_IMMEDIATE",
		"B_IMMEDIATE_LOCAL",
		"B_REG",
		"B_REG_OFFSET",
		"LOAD_IMMEDIATE_DWORD",
		"LOAD_IMMEDIATE_WORD",
		"LOAD_IMMEDIATE_BYTE",
		"LOAD_IMMEDIATE_OFFSET",
		"LOAD_SCALED_REG_OFFSET",
		"STORE_IMMEDIATE_OFFSET",
		"STORE_SCALED_REG_OFFSET",
		"LOAD_MULTIPLE",
		"LOAD_MULTIPLE_S",
		"STORE_MULTIPLE",
		"STORE_MULTIPLE_S",
		"DATA_PROCESSING_IMM",
		"DATA_PROCESSING_REG",
		"DATA_PROCESSING_IMM_S",
		"DATA_PROCESSING_REG_S",
		"DATA_PROCESSING_IMM_SHIFT",
		"DATA_PROCESSING_REG_SHIFT",
		"MOV_IMM",
		"MOV_IMM_NZ",
		"MOV_REG",
		"CMP_IMM_S",
		"CMP_REG_S",
		"CMN_REG_S",
		"TST_REG_S",
		"ADD_IMM",
		"ADD_IMM_S",
		"ADD_REG",
		"ADD_REG_S",
		"ADC_REG_S",
		"SUB_REG_S",
		"SBC_REG_S",
		"AND_IMM",
		"ORR_IMM",
		"ORR_REG_S",
		"LSL_IMM",
		"LSL_IMM_S",
		"LSL_REG",
		"LSL_REG_S",
		"LSR_IMM",
		"LSR_IMM_S",
		"LSR_REG",
		"LSR_REG_S",
		"ASR_IMM",
		"ASR_IMM_S",
		"ASR_REG",
		"ASR_REG_S",
		"ROR_REG",
		"ROR_REG_S",
		"AND_REG_S",
		"EOR_REG_S",
		"BIC_REG_S",
		"NEG_REG_S",
		"MVN_REG_S",
		"MULTIPLY",
		"MULTIPLY_LONG",
		"SWAP",
		"COUNT_LEADING_ZEROS",
		"MOVE_TO_SR_IMM",
		"MOVE_TO_SR_REG",
		"MOVE_FROM_SR",
		"UNDEFINED",
		"SWI",
		"BKPT",
		"COPROC_REG_TRANSFER",
		"COPROC_DOUBLE_REG_TRANSFER",
		"COPROC_DATA_PROCESSING",
		"COPROC_LOAD_STORE",
		"MAX_UOP_OPCODE",
		"UNKNOWN OPCODE"
		"UNKNOWN OPCODE"
		"UNKNOWN OPCODE"
};

#define MAX_UOP_OPCODE 75
const char *uop_opc_to_str(int opc);
const char *uop_opc_to_str(int opc)
{
	if(opc > MAX_UOP_OPCODE)
		opc=MAX_UOP_OPCODE;
	return opc_str[opc];
}
/*================================================================*/
/* end of uop_opc.c */
