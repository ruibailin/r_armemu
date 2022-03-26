
/*------------------------------------
 * isa_cpu.c
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
#include "isa_cpu.h"

struct isa_struct isa;
/*================================================================*/
enum arm_core get_core();
enum arm_core get_core()
{
	return isa.core;
}

enum arm_instruction_set get_isa();
enum arm_instruction_set get_isa()
{
	return isa.isa;
}

/*================================================================*/
#include "../1imp.h"
bool isa_7_core(void);
bool isa_7_core(void)
{
	return (isa.core == ARM7);
}

bool isa_9_core(void);
bool isa_9_core(void)
{
	return (isa.core == ARM9);
}

bool isa_9e_core(void);
bool isa_9e_core(void)
{
	return (isa.core == ARM9e);
}

/*================================================================*/
bool isa_v4_iset(void);
bool isa_v4_iset(void)
{
	return (isa.isa == ARM_V4);
}

bool isa_v5_iset(void);
bool isa_v5_iset(void)
{
	return (isa.isa == ARM_V5);
}

bool isa_v5e_iset(void);
bool isa_v5e_iset(void)
{
	return (isa.isa == ARM_V5e);
}

bool isa_v6_iset(void);
bool isa_v6_iset(void)
{
	return (isa.isa == ARM_V6);
}
/*================================================================*/
struct cpu_types {
    const char *name;
    enum arm_instruction_set isa;
    enum arm_core core;
    bool with_cp15;
    bool with_mmu;
};

static const struct cpu_types cpu_types_val[] = {
    /* name,   isa,    core, cp15?, mmu? */
    { "armv4",     ARM_V4,  ARM7,  FALSE, FALSE },
    { "armv5",     ARM_V5,  ARM9,  TRUE,  TRUE },
    { "armv5e",    ARM_V5e, ARM9,  TRUE,  TRUE },
    { "armv6",     ARM_V6,  ARM9,  TRUE,  TRUE }, // not correct, but no arm11 support yet

    { "arm7tdmi",  ARM_V4,  ARM7,  FALSE, FALSE },
    { "arm7",      ARM_V4,  ARM7,  FALSE, FALSE },
    { "arm9tdmi",  ARM_V4,  ARM9,  TRUE,  TRUE },
    { "arm9",      ARM_V4,  ARM9,  TRUE,  TRUE },
    { "arm9e",     ARM_V5e, ARM9e, TRUE,  TRUE },
    { "arm926ejs", ARM_V5e, ARM9e, TRUE,  TRUE },
    { "arm926",    ARM_V5e, ARM9e, TRUE,  TRUE },

    { NULL, 0, 0, 0, 0 },
};

#include "../cop/cop.h"
#include "../mmu/mmu.h"
void isa_init_cpu(const char *cpu_type);
void isa_init_cpu(const char *cpu_type)
{
	const struct cpu_types *t;

	//default settings
    isa.isa = ARM_V4;
    isa.core = ARM7;

    if (cpu_type==NULL)
    return;

    for (t = cpu_types_val; t->name; t++) {
    	if(t->name == NULL)
    		break;
        if (os_strcasecmp(t->name, cpu_type))
        	continue;

        isa.isa = t->isa;
        isa.core = t->core;

        // initialize cp15
        if (t->with_cp15 )
            cop_init_cop15();

        // initialize mmu
        mmu_init_mmu(t->with_mmu);
        break;
    }
}
/*================================================================*/
/* end of isa_cpu.c */
