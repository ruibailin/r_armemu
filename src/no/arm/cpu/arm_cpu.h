
/*------------------------------------
 * cpu_arm.h
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
#ifndef NO_ARM_CPU_ARM_H_
#define NO_ARM_CPU_ARM_H_
/*================================================================*/
#include "../1imp.h"
/*================================================================*/
extern void cpu_set_cpsr(word condition, bool set);
extern void cpu_set_cpsr_NZ(reg_t val);
extern word cpu_get_cpsr(word condition);

//Old and confused function name
extern void set_condition(word condition, bool set);
extern void set_NZ_condition(reg_t val);
extern word get_condition(word condition);

/*================================================================*/
extern reg_t cpu_get_reg(int num);
extern void cpu_put_reg(int num, reg_t data);
extern void cpu_put_nopc_reg(int num, reg_t data);
extern bool cpu_is_in_priv(void);

//Old and confused function name
extern reg_t get_reg(int num);
extern void put_reg(int num, reg_t data);
extern void put_reg_nopc(int num, reg_t data);
extern bool arm_in_priviledged(void);
/*================================================================*/
/* Misc function prototypes */
extern int initialize_cpu(void);
extern void reset_cpu(void);
extern int start_cpu(void);
extern void panic_cpu(const char *fmt, ...);
extern void shutdown_cpu(void);
extern void dump_cpu(void);
extern void dump_registers(void);

//let others know all functions come from cpu module
extern int  cpu_init_cpu(void);
extern void cpu_reset(void);
extern int  cpu_start(void);
extern void cpu_panic(const char *fmt, ...);
extern void cpu_shutdown(void);
extern void cpu_dump_all(void);
extern void cpu_trace_dump(void);
extern void cpu_dump_reg(void);
/*================================================================*/
/* function prototypes for CPU */
extern void set_cpu_mode(int mode);
extern reg_t get_reg_user(int num);            /* "user" mode reg access */
extern void put_reg_user(int num, reg_t data); /* same */

/* function prototypes for CPU */
extern void cpu_set_mode(int mode);
extern reg_t cpu_get_user_reg(int num);            /* "user" mode reg access */
extern void cpu_put_user_reg(int num, reg_t data); /* same */
/*================================================================*/
/* exceptions and interruption */
extern void set_exception_base(armaddr_t base);
extern void raise_irq(void);
extern void lower_irq(void);
extern void raise_fiq(void);
extern void lower_fiq(void);
extern void signal_data_abort(armaddr_t addr);
extern void signal_prefetch_abort(armaddr_t addr);
extern bool process_pending_exceptions(void);

//let others know all functions come from cpu module
extern void cpu_init_exception(armaddr_t base);
extern void cpu_raise_irq(void);
extern void cpu_lower_irq(void);
extern void cpu_raise_fiq(void);
extern void cpu_lower_fiq(void);
extern void bus_raise_irq(void);
extern void bus_lower_irq(void);
extern void bus_raise_fiq(void);
extern void bus_lower_fiq(void);
extern void cpu_send_data_abort(armaddr_t addr);
extern void cpu_send_pref_abort(armaddr_t addr);
extern bool cpu_deal_exception(void);
/*================================================================*/
//Defined in other module
/* coprocessor 15 is for system mode stuff */
#include "../cop/cop.h"
extern void install_coprocessor(int cp_num, struct arm_coprocessor *coproc);
extern void install_cp15(void);

extern void cop_init_cop(int cp_num, struct arm_coprocessor *coproc);
extern void cop_init_cop15(void);
/* codepage maintenance */
extern void flush_all_codepages(void); /* throw away all cached instructions */

extern void cdp_flush_all(void);

/* condition table */
extern int build_condition_table(void);

extern int cdt_build_table(void);

/*================================================================*/
#endif 
/* end of cpu_arm.h */
