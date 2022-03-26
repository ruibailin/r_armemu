
/*------------------------------------
 * cop_cpu.c
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
#include "../1imp.h"
/*================================================================*/
#include "cop_cpu.h"

struct system_coproc {
    dword id; // cr0
    dword cr1; // cr1
    dword process_id; // cr13
};

static struct system_coproc cp15;
/*================================================================*/
#include "../cpu/cpu.h"
#include "../mmu/mmu.h"
#include "../pfm/pfm.h"
/*================================================================*/
static void op_cp15_reg_transfer(dword ins, void *data);
static void op_cp15_reg_transfer(dword ins, void *data)
{
    int opcode_1, opcode_2;
    int CRn, CRm;
    int Rd;
    int L;
    int cp_num;
    reg_t val;

    // decode the fields in the instruction
    CRm = BITS(ins, 3, 0);
    opcode_2 = BITS_SHIFT(ins, 7, 5);
    cp_num = BITS_SHIFT(ins, 11, 8);
    Rd = BITS_SHIFT(ins, 15, 12);
    CRn = BITS_SHIFT(ins, 19, 16);
    L = BIT_SET(ins, 20);
    opcode_1 = BITS_SHIFT(ins, 23, 21);

    CPU_TRACE(5, "\t\top_cp15_reg_transfer: cp_num %d L %d Rd %d CRn %d CRm %d op1 %d op2 %d\n",
              cp_num, L, Rd, CRn, CRm, opcode_1, opcode_2);

    pfm_cnt_cop_reg();

    /* some coprocessors we dont care about */
    switch (CRn) {
        case 0: // id register
            if (L) {
                val = cp15.id;
                goto loadval;
            }
            goto done;
        case 1: // system control register
            if (L) {
                val = cp15.cr1;
                goto loadval;
            } else {
                dword newval = get_reg(Rd);

                if (newval & (1<<15)) { // armv5 backwards compatibility mode
                    panic_cpu("backwards compatible PC load mode not supported in armv5 (bit 15 in cr1)\n");
                }
                /* ignore all the other bits */

                /* set high/low vector base */
                set_exception_base((newval & (1<<13)) ? 0xffff0000 : 0);

                /* load the potentially new mmu config */
                mmu_set_flags(newval & MMU_FLAG_MASK);

                /* save our config */
                cp15.cr1 = newval;

                goto done;
            }
        case 2: // translation table base
            if (L) {
                val = mmu_get_register(MMU_TRANS_TABLE_REG);
                goto loadval;
            } else {
                mmu_set_register(MMU_TRANS_TABLE_REG, get_reg(Rd));
                goto done;
            }
        case 3: // domain access control
            if (L) {
                val = mmu_get_register(MMU_DOMAIN_ACCESS_CONTROL_REG);
                goto loadval;
            } else {
                mmu_set_register(MMU_DOMAIN_ACCESS_CONTROL_REG, get_reg(Rd));
                goto done;
            }
        case 4: // unpredictable
            goto donothing;
        case 5: // fault state register
            if (L) {
                val = mmu_get_register(MMU_FAULT_STATUS_REG);
                goto loadval;
            } else {
                mmu_set_register(MMU_FAULT_STATUS_REG, get_reg(Rd));
                goto done;
            }
        case 6: // fault address register
            if (L) {
                val = mmu_get_register(MMU_FAULT_ADDRESS_REG);
                goto loadval;
            } else {
                mmu_set_register(MMU_FAULT_ADDRESS_REG, get_reg(Rd));
                goto done;
            }
        case 7: // cache registers, we only sort of emulate the instruction cache
            CPU_TRACE(5, "cache instruction: L %d m %d n %d d %d op1 %d op2 %d\n", L, CRm, CRn, Rd, opcode_1, opcode_2);
            if (!L) {
                switch (CRm) {
                    case 0: // wait for interrupt
                        goto done;
                    case 5: // various forms of ICache invalidation
                    case 7: // invalidate Icache + Dcache
                        flush_all_codepages();
                        goto done;
                    case 6: // invalidate dcache
                        goto done;
                    case 10: // clean dcache & drain write buffer
                        goto done;
                    case 13: // prefetch icache
                        goto done;
                    case 14: // clean and invalidate dcache
                        goto done;
                }
            } else { // store
                switch (CRm) {
                    case 14: // clean and invalidate dcache
                    case 10: // clean dcache & drain write buffer
                        // arm926 special cache routine that makes it easy to clear the entire cache
                        // since it doesn't really do anything, probably okay to leave it in for all cores
                        if (opcode_2 == 3) {
                            // test and clean, side effect is to set the NZ condition
                            val = 0; // (1<<30);
                            set_NZ_condition(val);
                            goto loadval;
                        }
                }
                goto donothing;
            }
            break;
        case 8: // tlb flush
            if (L) {
                switch (CRm) {
                    case 7: // unified TLB
                    case 5: // instruction TLB
                        flush_all_codepages();
                    // fall through
                    case 6: // data TLB
                        mmu_invalidate_tcache();
                        goto done;
                }
            }
            goto donothing;
        case 9: // cache lockdown
            goto donothing;
        case 10: // tlb lockdown
            goto donothing;
        case 11:
        case 12: // unpredictable
            goto donothing;
        case 13: // process id register
            if (L) {
                val = cp15.process_id;
                goto loadval;
            } else {
                cp15.process_id = get_reg(Rd);
                goto done;
            }
        case 14: // reserved
            goto donothing;
        case 15: // test and debug
            goto donothing;
        default:
            goto donothing;
    }

loadval:
    if (L && Rd != 15)
        put_reg(Rd, val);
    goto done;

unsupported:
    panic_cpu("reading from or writing to unsupported cp15 control register!\n");

donothing:
    if (L && Rd != 15) {
        put_reg(Rd, 0);
    }

done:
    ;
}

static void op_cp15_double_reg_transfer(dword ins, void *data);
static void op_cp15_double_reg_transfer(dword ins, void *data)
{
    int CRm;
    int Rn, Rd;
    int opcode;
    int cp_num;
    int L;

    // decode the fields in the instruction
    CRm = BITS(ins, 3, 0);
    opcode = BITS_SHIFT(ins, 7, 4);
    cp_num = BITS_SHIFT(ins, 11, 8);
    Rd = BITS_SHIFT(ins, 15, 12);
    Rn = BITS_SHIFT(ins, 19, 16);
    L = BIT_SET(ins, 20);

    CPU_TRACE(5, "\t\top_cp15_double_reg_transfer: cp_num %d L %d Rd %d CRn %d CRm %d opcode %d\n",
              cp_num, L, Rd, CRm, opcode);

    panic_cpu("op_co_double_reg_transfer unimplemented\n");
    CPU_TRACE(1, "warning, ignoring coprocessor instruction\n");

    pfm_cnt_cop_reg();
}

static void op_cp15_data_processing(dword ins, void *data);
static void op_cp15_data_processing(dword ins, void *data)
{
    int opcode_1, opcode_2;
    int CRn, CRm;
    int CRd;
    int cp_num;

    // decode the fields in the instruction
    CRm = BITS(ins, 3, 0);
    opcode_2 = BITS_SHIFT(ins, 7, 5);
    cp_num = BITS_SHIFT(ins, 11, 8);
    CRd = BITS_SHIFT(ins, 15, 12);
    CRn = BITS_SHIFT(ins, 19, 16);
    opcode_1 = BITS_SHIFT(ins, 23, 20);

    CPU_TRACE(5, "\t\top_cp15_data_processing: cp_num %d CRd %d CRn %d CRm %d op1 %d op2 %d\n",
              cp_num, CRd, CRn, CRm, opcode_1, opcode_2);

    panic_cpu("op_co_data_processing unimplemented\n");
    CPU_TRACE(1, "warning, ignoring coprocessor instruction\n");

    pfm_cnt_cop_data();

}

static void op_cp15_load_store(dword ins, void *data);
static void op_cp15_load_store(dword ins, void *data)
{
    int CRd;
    int Rn;
    int cp_num;
    int P, U, N, W, L;
    int offset_8;

    // decode the fields in the instruction
    offset_8 = BITS(ins, 7, 0);
    cp_num = BITS_SHIFT(ins, 11, 8);
    CRd = BITS_SHIFT(ins, 15, 12);
    Rn = BITS_SHIFT(ins, 19, 16);
    L = BIT_SET(ins, 20);
    W = BIT_SET(ins, 21);
    N = BIT_SET(ins, 22);
    U = BIT_SET(ins, 23);
    P = BIT_SET(ins, 24);

    CPU_TRACE(5, "\t\top_cp15_load_store: cp_num %d L %d W %d N %d U %d P %d Rn %d CRd %d offset_8 %d\n",
              cp_num, L, W, N, U, P, Rn, CRd, offset_8);

    panic_cpu("op_co_load_store unimplemented\n");
    CPU_TRACE(1, "warning, ignoring coprocessor instruction\n");

    pfm_cnt_cop_ld_st();
}


/*================================================================*/
struct cop_struct cop;
void cop_init_cop(int cp_num, struct arm_coprocessor *coproc);
void cop_init_cop(int cp_num, struct arm_coprocessor *coproc)
{
    if (cp_num < 0 || cp_num > 15)
        panic_cpu("install_coprocessor: bad cp num %d\n", cp_num);

    os_memcpy(&cop.coproc[cp_num], coproc, sizeof(struct arm_coprocessor));
}


void cop_init_cop15(void);
void cop_init_cop15(void)
{
    struct arm_coprocessor cp15_coproc;

    cp15_coproc.installed = 1;
    cp15_coproc.data = (void *)&cp15;

    cp15_coproc.reg_transfer = op_cp15_reg_transfer;
    cp15_coproc.double_reg_transfer = op_cp15_double_reg_transfer;
    cp15_coproc.data_processing = op_cp15_data_processing;
    cp15_coproc.load_store = op_cp15_load_store;

    cop_init_cop(15, &cp15_coproc);
}

/*================================================================*/
//To be compatible to old version
void install_coprocessor(int cp_num, struct arm_coprocessor *coproc);
void install_coprocessor(int cp_num, struct arm_coprocessor *coproc)
{
    cop_init_cop(cp_num, coproc);
}

void install_cp15(void);
void install_cp15(void)
{
	cop_init_cop15();
}

/*================================================================*/

/* end of cop_cpu.c */
