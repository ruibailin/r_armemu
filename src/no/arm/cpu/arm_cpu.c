
/*------------------------------------
 * cpu_arm.c
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

#include "0ctr.h"
/*================================================================*/
#include "../1imp.h"

#include "cpu_data.h"
struct cpu_struct cpu; // the cpu
/*================================================================*/
#include "../isa/isa.h"
#include "../mmu/mmu.h"
#include "../cdt/cdt.h"
#include "../cdp/cdp.h"
/*================================================================*/
#include "cpu_psr.h"
void cpu_set_cpsr(dword condition, bool set);
void cpu_set_cpsr(dword condition, bool set)
{
    if (condition == PSR_THUMB) {
        CPU_TRACE(7, "setting THUMB bit to %d\n", set);
    }

    if (set)
        cpu.cpsr |= condition;
    else
        cpu.cpsr &= ~condition;
}

#include "cpu_bits.h"
void cpu_set_cpsr_NZ(reg_t val);
void cpu_set_cpsr_NZ(reg_t val)
{
    cpu_set_cpsr(PSR_CC_NEG, BIT(val, 31));
    cpu_set_cpsr(PSR_CC_ZERO, val == 0);
}

dword cpu_get_cpsr(dword condition);
dword cpu_get_cpsr(dword condition)
{
    return (cpu.cpsr & condition);
}


#include "cpu_psr.h"
void set_condition(dword condition, bool set);
void set_condition(dword condition, bool set)
{
    if (condition == PSR_THUMB) {
        CPU_TRACE(7, "setting THUMB bit to %d\n", set);
    }

    if (set)
        cpu.cpsr |= condition;
    else
        cpu.cpsr &= ~condition;
}

#include "cpu_bits.h"
void set_NZ_condition(reg_t val);
void set_NZ_condition(reg_t val)
{
    set_condition(PSR_CC_NEG, BIT(val, 31));
    set_condition(PSR_CC_ZERO, val == 0);
}

dword get_condition(dword condition);
dword get_condition(dword condition)
{
    return (cpu.cpsr & condition);
}
/*================================================================*/
reg_t cpu_get_reg(int num);
reg_t cpu_get_reg(int num)
{
    ASSERT(num >= 0 && num < 16);
    return cpu.r[num];
}

reg_t get_reg(int num);
reg_t get_reg(int num)
{
    ASSERT(num >= 0 && num < 16);
    return cpu.r[num];
}

/*================================================================*/
#include "cpu_sreg.h"
void cpu_put_reg(int num, reg_t data);
void cpu_put_reg(int num, reg_t data)
{
    ASSERT(num >= 0 && num < 16);
    cpu.r[num] = data;
    if (num == PC) {
        cpu.r15_dirty = TRUE; // on the next loop, resync the "real" pc (cpu.pc) with cpu.r[15]
        cpu.r[PC] &= ~1;
    }
}

void put_reg(int num, reg_t data);
void put_reg(int num, reg_t data)
{
    ASSERT(num >= 0 && num < 16);
    cpu.r[num] = data;
    if (num == PC) {
        cpu.r15_dirty = TRUE; // on the next loop, resync the "real" pc (cpu.pc) with cpu.r[15]
        cpu.r[PC] &= ~1;
    }
}
/*================================================================*/
void cpu_put_nopc_reg(int num, reg_t data);
void cpu_put_nopc_reg(int num, reg_t data)
{
    ASSERT(num >= 0 && num < 15);
    cpu.r[num] = data;
}

void put_reg_nopc(int num, reg_t data);
void put_reg_nopc(int num, reg_t data)
{
    ASSERT(num >= 0 && num < 15);
    cpu.r[num] = data;
}
/*================================================================*/
bool cpu_is_in_priv(void);
bool cpu_is_in_priv(void)
{
    return ((cpu.cpsr & PSR_MODE_MASK) != PSR_MODE_user);
}

bool arm_in_priviledged(void);
bool arm_in_priviledged(void)
{
    return ((cpu.cpsr & PSR_MODE_MASK) != PSR_MODE_user);
}

/*================================================================*/
int cpu_init_cpu(void);
int cpu_init_cpu(void)
{
    os_memclr(&cpu, sizeof(cpu));

    return 0;
}

int initialize_cpu(void);
int initialize_cpu(void)
{
    cpu_init_cpu();
    return 0;
}

#include "cpu_ex.h"
void cpu_reset(void);
void cpu_reset(void)
{
    atomic_or(&cpu.pending_exceptions, EX_RESET); // schedule a reset
}

void reset_cpu(void);
void reset_cpu(void)
{
	cpu_reset();
}

int cpu_start(void);
int cpu_start(void)
{
    // start the uop engine
//    uop_dispatch_loop();
    // run the decoder loop
//    decoder_loop();
    return 0;
}


int start_cpu(void);
int start_cpu(void)
{
   cpu_start();
   return 0;
}


void cpu_shutdown(void);
void cpu_shutdown(void)
{
}

void shutdown_cpu(void);
void shutdown_cpu(void)
{
	cpu_shutdown();
}

#include "../pfm/pfm.h"
void cpu_dump_all(void);
void cpu_dump_all(void)
{
    os_printf("cpu_dump: ins %d\n", get_instruction_count());
    os_printf("r0:   0x%08x r1:   0x%08x r2:   0x%08x r3:   0x%08x\n", cpu.r[0], cpu.r[1], cpu.r[2], cpu.r[3]);
    os_printf("r4:   0x%08x r5:   0x%08x r6:   0x%08x r7:   0x%08x\n", cpu.r[4], cpu.r[5], cpu.r[6], cpu.r[7]);
    os_printf("r8:   0x%08x r9:   0x%08x r10:  0x%08x r11:  0x%08x\n", cpu.r[8], cpu.r[9], cpu.r[10], cpu.r[11]);
    os_printf("r12:  0x%08x sp:   0x%08x lr:   0x%08x r15:  0x%08x pc:   0x%08x\n", cpu.r[12], cpu.r[13], cpu.r[14], cpu.r[15], cpu.pc);
    os_printf("cpsr: 0x%08x (%c %c%c%c%c) spsr: 0x%08x\n",
           cpu.cpsr,
           get_condition(PSR_THUMB) ? 'T':' ',
           get_condition(PSR_CC_NEG) ? 'N':' ',
           get_condition(PSR_CC_ZERO) ? 'Z':' ',
           get_condition(PSR_CC_CARRY) ? 'C':' ',
           get_condition(PSR_CC_OVL) ? 'O':' ',
           cpu.spsr);
}

void cpu_trace_dump(void);
void cpu_trace_dump(void)
{
#if DYNAMIC_TRACE_LEVELS
    if(TRACE_CPU_LEVEL >=10)
    	cpu_dump_all();
#endif
}


void dump_cpu(void);
void dump_cpu(void)
{
	cpu_dump_all();
}

void cpu_dump_reg(void);
void cpu_dump_reg(void)
{

    os_printf("%08x %08x %08x %08x %08x %08x %08x %08x %c%c%c%c\n",
           cpu.r[0], cpu.r[1], cpu.r[2], cpu.r[3],
           cpu.r[4], cpu.r[5], cpu.r[6], cpu.r[7],
           get_condition(PSR_CC_NEG) ? 'N':'-',
           get_condition(PSR_CC_ZERO) ? 'Z':'-',
           get_condition(PSR_CC_CARRY) ? 'C':'-',
           get_condition(PSR_CC_OVL) ? 'O':'-'
          );
    os_printf("%08x %08x %08x %08x %08x %08x %08x %08x\n",
           cpu.r[8], cpu.r[9], cpu.r[10], cpu.r[11],
           cpu.r[12], cpu.r[13], cpu.r[14], cpu.pc);


}

void dump_registers(void);
void dump_registers(void)
{
	cpu_dump_reg();
}

void cpu_panic(const char *fmt, ...);
void cpu_panic(const char *fmt, ...)
{
    os_printf("panic: ");
    os_printf(fmt);
    cpu_dump_all();
    cpu_shutdown();
}

void panic_cpu(const char *fmt, ...);
void panic_cpu(const char *fmt, ...)
{
    os_printf("panic: ");

    dump_cpu();
    shutdown_cpu();
}
/*================================================================*/
#include  "cpu_psr.h"
void cpu_set_mode(int new_mode);
void cpu_set_mode(int new_mode)
{
    reg_t *bank;
    int old_mode;

    old_mode= cpu.cpsr & PSR_MODE_MASK;
    CPU_TRACE(4, "mode change: 0x%x to 0x%x\n", old_mode, new_mode);

    if (old_mode == new_mode)
        return;

    // save the regs from the mode we're coming from
    switch (old_mode) {
        case PSR_MODE_user:
        case PSR_MODE_sys:
            // usr and sys mode share the same "bank" of registers
            cpu.usr_regs[0] = cpu.r[13]; // sp
            cpu.usr_regs[1] = cpu.r[14]; // lr
            break;

        case PSR_MODE_fiq:
            // fiq mode has a seperate bank for r8-r14, and spsr
            os_memcpy(cpu.fiq_regs, &cpu.r[8], sizeof(reg_t) * 7); // r8-r14
            cpu.fiq_regs[7] = cpu.spsr;

            // we must be moving to a "user" mode so switch to the user r8-r12
            os_memcpy(&cpu.r[8], cpu.usr_regs_low, sizeof(reg_t) * 5); // r8-r12

            break;

        // the other 4 modes are similar, so select the bank and share the code
        case PSR_MODE_irq:
            bank = cpu.irq_regs;
            goto save_3reg;
        case PSR_MODE_svc:
            bank = cpu.svc_regs;
            goto save_3reg;
        case PSR_MODE_abt:
            bank = cpu.abt_regs;
            goto save_3reg;
        case PSR_MODE_und:
            bank = cpu.und_regs;
save_3reg:
            bank[0] = cpu.r[13]; // sp
            bank[1] = cpu.r[14]; // lr
            bank[2] = cpu.spsr;  // spsr
            break;
    }

    // restore the regs from the mode we're going to
    switch (new_mode) {
        case PSR_MODE_user:
        case PSR_MODE_sys:
            // usr and sys mode share the same "bank" of registers
            cpu.r[13] = cpu.usr_regs[0];
            cpu.r[14] = cpu.usr_regs[1];
            break;

        case PSR_MODE_fiq:
            // we must be coming from one of the "user" modes, so save r8-r12
            os_memcpy(cpu.usr_regs_low, &cpu.r[8], sizeof(reg_t) * 5); // r8-r12

            // fiq mode has a seperate bank for r8-r14, and spsr
            os_memcpy(&cpu.r[8], cpu.fiq_regs, sizeof(reg_t) * 7); // r8-r14
            cpu.spsr = cpu.fiq_regs[7];
            break;

        // the other 4 modes are similar, so select the bank and share the code
        case PSR_MODE_irq:
            bank = cpu.irq_regs;
            goto restore_3reg;
        case PSR_MODE_svc:
            bank = cpu.svc_regs;
            goto restore_3reg;
        case PSR_MODE_abt:
            bank = cpu.abt_regs;
            goto restore_3reg;
        case PSR_MODE_und:
            bank = cpu.und_regs;
restore_3reg:
            cpu.r[13] = bank[0]; // sp
            cpu.r[14] = bank[1]; // lr
            cpu.spsr = bank[2];  // spsr
            break;
    }
    // set the mode bits
    cpu.cpsr &= ~PSR_MODE_MASK;
    cpu.cpsr |= new_mode;
}

void set_cpu_mode(int new_mode);
void set_cpu_mode(int new_mode)
{
	cpu_set_mode(new_mode);
}

/*================================================================*/
/* access the "user" mode registers */
/* NOTE: undefined if the cpu is in USR/SYS mode */
reg_t cpu_get_user_reg(int num);
reg_t cpu_get_user_reg(int num)
{
    ASSERT(num >= 0 && num <= 15);

    if (num == 13)
        return cpu.usr_regs[0];

    if (num == 14)
        return cpu.usr_regs[1];

    if (num >= 8 && num <= 12 && (cpu.cpsr & PSR_MODE_MASK) == PSR_MODE_fiq)  /* r8-r12 and the cpu is in FIQ */
        return cpu.usr_regs_low[num - 8]; /* user regs are saved in this bank */

    return get_reg(num);
}

reg_t get_reg_user(int num);
reg_t get_reg_user(int num)
{
    ASSERT(num >= 0 && num <= 15);

    if (num == 13) {
        return cpu.usr_regs[0];
    } else if (num == 14) {
        return cpu.usr_regs[1];
    } else if (num >= 8 && num <= 12 && (cpu.cpsr & PSR_MODE_MASK) == PSR_MODE_fiq) { /* r8-r12 and the cpu is in FIQ */
        return cpu.usr_regs_low[num - 8]; /* user regs are saved in this bank */
    } else { /* all other regs access normally */
        return get_reg(num);
    }
}

/* same as get_reg_user */
void cpu_put_user_reg(int num, reg_t data);
void cpu_put_user_reg(int num, reg_t data)
{
    ASSERT(num >= 0 && num <= 15);

    if (num == 13)
    {
        cpu.usr_regs[0] = data;
        return;
    }

    if (num == 14)
    {
        cpu.usr_regs[1] = data;
        return;
    }

    if (num >= 8 && num <= 12 && (cpu.cpsr & PSR_MODE_MASK) == PSR_MODE_fiq)
    { /* r8-r12 and the cpu is in FIQ */
        cpu.usr_regs_low[num - 8] = data; /* user regs are saved in this bank */
        return;
    }

    /* all other regs access normally */
    put_reg(num, data);
    return;
}

/* same as get_reg_user */
void put_reg_user(int num, reg_t data);
void put_reg_user(int num, reg_t data)
{
    ASSERT(num >= 0 && num <= 15);

    if (num == 13) {
        cpu.usr_regs[0] = data;
    } else if (num == 14) {
        cpu.usr_regs[1] = data;
    } else if (num >= 8 && num <= 12 && (cpu.cpsr & PSR_MODE_MASK) == PSR_MODE_fiq) { /* r8-r12 and the cpu is in FIQ */
        cpu.usr_regs_low[num - 8] = data; /* user regs are saved in this bank */
    } else { /* all other regs access normally */
        put_reg(num, data);
    }
}

/*================================================================*/
void cpu_init_exception(armaddr_t addr);
void cpu_init_exception(armaddr_t addr)
{
    CPU_TRACE(4, "set_exception_base: 0x%08x\n", addr);
    cpu.exception_base = addr;
}

void set_exception_base(armaddr_t addr);
void set_exception_base(armaddr_t addr)
{
	cpu_init_exception(addr);
}

void cpu_raise_irq(void);
void cpu_raise_irq(void)
{
    CPU_TRACE(5, "raise_irq\n");
    atomic_or(&cpu.pending_exceptions, EX_IRQ);
}

void cpu_lower_irq(void);
void cpu_lower_irq(void)
{
    CPU_TRACE(5, "lower_irq\n");
    atomic_and(&cpu.pending_exceptions, ~EX_IRQ);
}

void cpu_raise_fiq(void);
void cpu_raise_fiq(void)
{
    CPU_TRACE(5, "raise_fiq\n");
    atomic_or(&cpu.pending_exceptions, EX_FIQ);
}

void cpu_lower_fiq(void);
void cpu_lower_fiq(void)
{
    CPU_TRACE(5, "lower_fiq\n");
    atomic_and(&cpu.pending_exceptions, ~EX_FIQ);
}

void bus_raise_irq(void);
void bus_raise_irq(void)
{
	cpu_raise_irq();
}

void bus_lower_irq(void);
void bus_lower_irq(void)
{
	cpu_lower_irq();
}

void bus_raise_fiq(void);
void bus_raise_fiq(void)
{
	cpu_raise_fiq();
}

void bus_lower_fiq(void);
void bus_lower_fiq(void)
{
	cpu_lower_fiq();
}

void raise_irq(void);
void raise_irq(void)
{
	cpu_raise_irq();
}

void lower_irq(void);
void lower_irq(void)
{
	cpu_lower_irq();
}

void raise_fiq(void);
void raise_fiq(void)
{
	cpu_raise_fiq();
}

void lower_fiq(void);
void lower_fiq(void)
{
	cpu_lower_fiq();
}

void cpu_send_data_abort(armaddr_t addr);
void cpu_send_data_abort(armaddr_t addr)
{
    CPU_TRACE(4, "data abort at 0x%08x\n", addr);
    atomic_or(&cpu.pending_exceptions, EX_DATA_ABT);
}

void signal_data_abort(armaddr_t addr);
void signal_data_abort(armaddr_t addr)
{
	cpu_send_data_abort(addr);
}

void cpu_send_pref_abort(armaddr_t addr);
void cpu_send_pref_abort(armaddr_t addr)
{
    CPU_TRACE(4, "prefetch abort at 0x%08x\n", addr);
    atomic_or(&cpu.pending_exceptions, EX_PREFETCH);
}


void signal_prefetch_abort(armaddr_t addr);
void signal_prefetch_abort(armaddr_t addr)
{
	cpu_send_pref_abort(addr);
}

/*================================================================*/
#include "cpu_sreg.h"

static bool cpu_deal_system_reset(void);
static bool cpu_deal_system_reset(void)
{
    // system reset
    if (!(cpu.pending_exceptions & EX_RESET))
    	return FALSE;
    // go to a default state
    cpu.cpsr = PSR_IRQ_MASK | PSR_FIQ_MASK;
    put_reg(PC, cpu.exception_base + 0x0);
    cdp.curr_cp = NULL;

    set_cpu_mode(PSR_MODE_svc);

    // mask all other pending exceptions except for irq or fiq
    atomic_and(&cpu.pending_exceptions, (EX_FIQ|EX_IRQ));

    CPU_TRACE(3, "EX: cpu reset!\n");
    pfm_cnt_exp();
    return TRUE;
}

static bool cpu_deal_undefined_ins(void);
static bool cpu_deal_undefined_ins(void)
{
    // undefined instruction
    if (!(cpu.pending_exceptions & EX_UNDEFINED))
    	return FALSE;

    cpu.und_regs[1] = cpu.pc + (get_condition(PSR_THUMB) ? 1 : 0); // next instruction after the undefined instruction
    cpu.und_regs[2] = cpu.cpsr;
    put_reg(PC, cpu.exception_base + 0x4);

    if (get_condition(PSR_THUMB))
        cdp.curr_cp = NULL; // reset the codepage
    set_condition(PSR_THUMB, FALSE); // move to arm
    set_condition(PSR_IRQ_MASK, TRUE);
    set_cpu_mode(PSR_MODE_und);

    atomic_and(&cpu.pending_exceptions, ~EX_UNDEFINED);

    CPU_TRACE(3, "EX: undefined instruction at 0x%08x\n", cpu.und_regs[1] - 4);
    pfm_cnt_exp();
    return TRUE;
}

static bool cpu_deal_swi_ins(void);
static bool cpu_deal_swi_ins(void)
{
    // SWI instruction
    if (!(cpu.pending_exceptions & EX_SWI))
    	return FALSE;

    cpu.svc_regs[1] = cpu.pc + (get_condition(PSR_THUMB) ? 1 : 0); // next instruction after the swi instruction
    cpu.svc_regs[2] = cpu.cpsr;
    put_reg(PC, cpu.exception_base + 0x8);

    if (get_condition(PSR_THUMB))
        cdp.curr_cp = NULL; // reset the codepage
    set_condition(PSR_THUMB, FALSE); // move to arm
    set_condition(PSR_IRQ_MASK, TRUE);
    set_cpu_mode(PSR_MODE_svc);

    atomic_and(&cpu.pending_exceptions, ~EX_SWI);

    CPU_TRACE(5, "EX: swi\n");
    pfm_cnt_exp();
    return TRUE;
}

static bool cpu_deal_pref_abort(void);
static bool cpu_deal_pref_abort(void)
{
    // prefetch abort
    if (!(cpu.pending_exceptions & EX_PREFETCH))
    	return FALSE;

	cpu.abt_regs[1] = cpu.pc + 4 + (get_condition(PSR_THUMB) ? 1 : 0); // next instruction after the aborted instruction
    cpu.abt_regs[2] = cpu.cpsr;
    put_reg(PC, cpu.exception_base + 0xc);

    if (get_condition(PSR_THUMB))
        cdp.curr_cp = NULL; // reset the codepage
    set_condition(PSR_THUMB, FALSE); // move to arm
    set_condition(PSR_IRQ_MASK, TRUE);
    set_cpu_mode(PSR_MODE_abt);

    atomic_and(&cpu.pending_exceptions, ~EX_PREFETCH);

    CPU_TRACE(4, "EX: prefetch abort\n");
    pfm_cnt_exp();
    return TRUE;
}

static bool cpu_deal_data_abort(void);
static bool cpu_deal_data_abort(void)
{
    // data abort
    if (!(cpu.pending_exceptions & EX_DATA_ABT))
    	return FALSE;

    cpu.abt_regs[1] = cpu.pc + 4 + (get_condition(PSR_THUMB) ? 1 : 0); // +8 from faulting instruction
    cpu.abt_regs[2] = cpu.cpsr;
    put_reg(PC, cpu.exception_base + 0x10);

    if (get_condition(PSR_THUMB))
        cdp.curr_cp = NULL; // reset the codepage
    set_condition(PSR_THUMB, FALSE); // move to arm
    set_condition(PSR_IRQ_MASK, TRUE);
    set_cpu_mode(PSR_MODE_abt);

    atomic_and(&cpu.pending_exceptions, ~EX_DATA_ABT);

    CPU_TRACE(4, "EX: data abort\n");
    pfm_cnt_exp();
    return TRUE;
}

static bool cpu_deal_fiq(void);
static bool cpu_deal_fiq(void)
{
    // fiq
    if (!(cpu.pending_exceptions & EX_FIQ))
    	return FALSE;
    if (cpu.cpsr & PSR_FIQ_MASK)
    	return FALSE;

    cpu.fiq_regs[6] = cpu.pc + 4 + (get_condition(PSR_THUMB) ? 1 : 0); // address of next instruction + 4
    cpu.fiq_regs[7] = cpu.cpsr;
    put_reg(PC, cpu.exception_base + 0x1c);

    if (get_condition(PSR_THUMB))
        cdp.curr_cp = NULL; // reset the codepage
    set_condition(PSR_THUMB, FALSE); // move to arm
    set_condition(PSR_IRQ_MASK, TRUE);
    set_cpu_mode(PSR_MODE_fiq);

    CPU_TRACE(5, "EX: FIQ\n");
    pfm_cnt_exp();
    return TRUE;
}

static bool cpu_deal_irq(void);
static bool cpu_deal_irq(void)
{
    // irq
    if (!(cpu.pending_exceptions & EX_IRQ))
    	return FALSE;
    if ((cpu.cpsr & PSR_IRQ_MASK))
    	return FALSE;

    cpu.irq_regs[1] = cpu.pc + 4 + (get_condition(PSR_THUMB) ? 1 : 0); // address of next instruction + 4
    cpu.irq_regs[2] = cpu.cpsr;
    put_reg(PC, cpu.exception_base + 0x18);

    if (get_condition(PSR_THUMB))
        cdp.curr_cp = NULL; // reset the codepage
    set_condition(PSR_THUMB, FALSE); // move to arm
    set_condition(PSR_IRQ_MASK, TRUE);
    set_cpu_mode(PSR_MODE_irq);

    CPU_TRACE(5, "EX: IRQ\n");
    pfm_cnt_exp();
    return TRUE;
}

bool cpu_deal_exceptions(void);
bool cpu_deal_exceptions(void)
{
    bool res;
	// a irq or fiq may happen asychronously in another thread
    CPU_TRACE(5, "process_pending_exceptions: pending ex 0x%x\n", cpu.pending_exceptions);

    res = cpu_deal_system_reset();
    if (res) return TRUE;
    res = cpu_deal_undefined_ins();
    if (res) return TRUE;
    res = cpu_deal_swi_ins();
    if (res) return TRUE;
    res=cpu_deal_pref_abort();
    if (res) return TRUE;
    res=cpu_deal_data_abort();
    if (res) return TRUE;
    res=cpu_deal_fiq();
    if (res) return TRUE;
    res=cpu_deal_irq();
    if (res) return TRUE;
    return FALSE;
}

// return value of true means something was processed, and thus a possible mode change
bool process_pending_exceptions(void);
bool process_pending_exceptions(void)
{
    bool res;
	// a irq or fiq may happen asychronously in another thread
    CPU_TRACE(5, "process_pending_exceptions: pending ex 0x%x\n", cpu.pending_exceptions);

    res = cpu_deal_system_reset();
    if (res) return TRUE;
    res = cpu_deal_undefined_ins();
    if (res) return TRUE;
    res = cpu_deal_swi_ins();
    if (res) return TRUE;
    res=cpu_deal_pref_abort();
    if (res) return TRUE;
    res=cpu_deal_data_abort();
    if (res) return TRUE;
    res=cpu_deal_fiq();
    if (res) return TRUE;
    res=cpu_deal_irq();
    if (res) return TRUE;
    return FALSE;
}

/*================================================================*/
/* end of cpu_arm.c */
