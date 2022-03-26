
/*------------------------------------
 * cdp_uop.c
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
#include "cdp_uop.h"
#include "../mmu/mmu.h"
#include "../cpu/cpu.h"

struct cdp_struct cdp;
/*================================================================*/
unsigned int codepage_hash(armaddr_t address, bool thumb);
unsigned int codepage_hash(armaddr_t address, bool thumb)
{
    unsigned int hash = (address / MMU_PAGESIZE);
    if (thumb)
        hash = ~hash; // XXX come up with something better
    return hash % CODEPAGE_HASHSIZE;
}/* codepage cache */

void free_codepage(struct uop_codepage *cp);
void free_codepage(struct uop_codepage *cp)
{
    UOP_TRACE(7, "free_codepage: cp %p, thumb %d, address 0x%x\n", cp, cp->thumb, cp->address);
    if (cp->thumb) {
        cp->next = cdp.free_cp_thumb;
        cdp.free_cp_thumb = cp;
    } else {
        cp->next = cdp.free_cp_arm;
        cdp.free_cp_arm = cp;
    }
}

/*================================================================*/
#define CP_PREALLOCATE 16
#define ARM_CP_SIZE (sizeof(struct uop_codepage) + sizeof(struct uop) * (NUM_CODEPAGE_INS_ARM + 1))
#define THUMB_CP_SIZE (sizeof(struct uop_codepage) + sizeof(struct uop) * (NUM_CODEPAGE_INS_THUMB + 1))
struct uop_codepage *alloc_codepage_arm(void);
struct uop_codepage *alloc_codepage_arm(void)
{
    struct uop_codepage *cp;

    if (cdp.free_cp_arm == NULL) {
        int i;

        uint8_t *cp_buf = os_malloc(ARM_CP_SIZE * CP_PREALLOCATE);
        if (cp_buf == NULL)
            return NULL;

        for (i=0; i < CP_PREALLOCATE; i++) {
            cp = (struct uop_codepage *)(cp_buf + (i * ARM_CP_SIZE));
            cp->thumb = FALSE;
            cp->pc_inc = 4;
            cp->pc_shift = 2;
            free_codepage(cp);
        }
    }

    ASSERT(cdp.free_cp_arm != NULL);
    cp = cdp.free_cp_arm;
    cdp.free_cp_arm = cp->next;
    cp->next = NULL;

    ASSERT(cp->thumb == FALSE);

    return cp;
}

struct uop_codepage *alloc_codepage_thumb(void);
struct uop_codepage *alloc_codepage_thumb(void)
{
    struct uop_codepage *cp;

    if (cdp.free_cp_thumb == NULL) {
        int i;

        uint8_t *cp_buf = os_malloc(THUMB_CP_SIZE * CP_PREALLOCATE);
        if (cp_buf == NULL)
            return NULL;

        for (i=0; i < CP_PREALLOCATE; i++) {
            cp = (struct uop_codepage *)(cp_buf + (i * THUMB_CP_SIZE));
            cp->thumb = TRUE;
            cp->pc_inc = 2;
            cp->pc_shift = 1;
            free_codepage(cp);
        }
    }

    ASSERT(cdp.free_cp_thumb != NULL);
    cp = cdp.free_cp_thumb;
    cdp.free_cp_thumb = cp->next;
    cp->next = NULL;

    ASSERT(cp->thumb == TRUE);
    return cp;
}

/*================================================================*/
struct uop_codepage *lookup_codepage(armaddr_t pc, bool thumb);
struct uop_codepage *lookup_codepage(armaddr_t pc, bool thumb)
{
    armaddr_t cp_addr = pc & ~(MMU_PAGESIZE-1);
    unsigned int hash;
    struct uop_codepage *cp;

    hash = codepage_hash(cp_addr, thumb);

    // search the hash chain for our codepage
    cp = cdp.codepage_hash[hash];
    while (cp != NULL) {
        if (cp->address == cp_addr && cp->thumb == thumb)
            return cp;
        cp = cp->next;
    }

    return NULL;
}

/*================================================================*/
#include "../uop/uop.h"
bool load_codepage_arm(armaddr_t pc, armaddr_t cp_addr, bool priviledged, struct uop_codepage **_cp, int *last_ins_index);
bool load_codepage_arm(armaddr_t pc, armaddr_t cp_addr, bool priviledged, struct uop_codepage **_cp, int *last_ins_index)
{
    struct uop_codepage *cp;
    int i;

    cp = alloc_codepage_arm();
    if (!cp)
        panic_cpu("could not allocate new codepage!\n");

    // load and fill in the default codepage
    cp->address = cp_addr;
    for (i=0; i < NUM_CODEPAGE_INS_ARM; i++) {
        cp->ops[i].opcode = DECODE_ME_ARM;
        cp->ops[i].cond = COND_AL;
        cp->ops[i].flags = 0;
        if (mmu_read_instruction_dword(cp_addr + i*4, &cp->ops[i].undecoded.raw_instruction, priviledged)) {
            UOP_TRACE(4, "load_codepage: mmu translation made arm codepage load fail\n");
            os_free(cp);
            return TRUE;
        }
    }
    *last_ins_index = NUM_CODEPAGE_INS_ARM;

    *_cp = cp;

    return FALSE;
}

bool load_codepage_thumb(armaddr_t pc, armaddr_t cp_addr, bool priviledged, struct uop_codepage **_cp, int *last_ins_index);
bool load_codepage_thumb(armaddr_t pc, armaddr_t cp_addr, bool priviledged, struct uop_codepage **_cp, int *last_ins_index)
{
    struct uop_codepage *cp;
    int i;

    cp = alloc_codepage_thumb();
    if (!cp)
        panic_cpu("could not allocate new codepage!\n");

    // load and fill in the default codepage
    cp->address = cp_addr;
    for (i=0; i < NUM_CODEPAGE_INS_THUMB; i++) {
        word hword;

        cp->ops[i].opcode = DECODE_ME_THUMB;
        cp->ops[i].cond = COND_AL;
        cp->ops[i].flags = 0;
        if (mmu_read_instruction_word(cp_addr + i*2, &hword, priviledged)) {
            UOP_TRACE(4, "load_codepage: mmu translation made thumb codepage load fail\n");
            os_free(cp);
            return TRUE;
        }
        cp->ops[i].undecoded.raw_instruction = hword;
    }
    *last_ins_index = NUM_CODEPAGE_INS_THUMB;

    *_cp = cp;

    return FALSE;
}

bool load_codepage(armaddr_t pc, bool thumb, bool priviledged, struct uop_codepage **_cp);
bool load_codepage(armaddr_t pc, bool thumb, bool priviledged, struct uop_codepage **_cp)
{
    armaddr_t cp_addr = pc & ~(MMU_PAGESIZE-1);
    struct uop_codepage *cp;
    unsigned int hash;
    int last_ins_index;
    bool ret;

    UOP_TRACE(4, "load_codepage: pc 0x%x\n", pc);

    // load and fill in the appropriate codepage
    if (thumb)
        ret = load_codepage_thumb(pc, cp_addr, priviledged, &cp, &last_ins_index);
    else
        ret = load_codepage_arm(pc, cp_addr, priviledged, &cp, &last_ins_index);
    if (ret)
        return TRUE; // there was some sort of error loading the new codepage

    // fill in the last instruction to branch to next codepage
    cp->ops[last_ins_index].opcode = B_IMMEDIATE;
    cp->ops[last_ins_index].cond = COND_AL;
    cp->ops[last_ins_index].flags = 0;
    cp->ops[last_ins_index].b_immediate.target = cp->address + MMU_PAGESIZE;
    cp->ops[last_ins_index].b_immediate.link_target = 0;
    cp->ops[last_ins_index].b_immediate.target_cp = NULL;

    // add it to the codepage hashtable
    hash = codepage_hash(cp_addr, thumb);
    cp->next = cdp.codepage_hash[hash];
    cdp.codepage_hash[hash] = cp;

    *_cp = cp;

    return FALSE;
}
/*================================================================*/
void cdp_init_cdp(void);
void cdp_init_cdp(void)
{
    os_memclr(cdp.codepage_hash, sizeof(cdp.codepage_hash));
    cdp.curr_cp = NULL;
}

bool cdp_set_cdp(armaddr_t pc);
bool cdp_set_cdp(armaddr_t pc)
{
    struct uop_codepage *cp;
    bool thumb = get_condition(PSR_THUMB) ? TRUE : FALSE;

    UOP_TRACE(6, "set_codepage: pc 0x%x thumb %d, real pc 0x%x\n", pc, thumb, cpu.pc);

    cp = lookup_codepage(cpu.pc, thumb);
    if (unlikely(cp == NULL)) { // still null, create a new one
        UOP_TRACE(7, "set_codepage: couldn't find codepage, creating new one\n");
        if (load_codepage(pc, thumb, arm_in_priviledged(), &cdp.curr_cp)) {
            return TRUE; // mmu fault
        }
    } else {
        UOP_TRACE(7, "set_codepage: found cached codepage\n");
        cdp.curr_cp = cp;
    }

    ASSERT(cdp.curr_cp != NULL);
    cpu.cp_pc = PC_TO_CPPC(cpu.pc);
    return FALSE;
}

void cdp_flush_all(void);
void cdp_flush_all(void)
{
    int i;
    struct uop_codepage *cp;

    /* XXX make this smarter */
    for (i=0; i < CODEPAGE_HASHSIZE; i++) {
        cp = cdp.codepage_hash[i];

        while (cp != NULL) {
            struct uop_codepage *temp = cp;
            cp = cp->next;
            free_codepage(temp);
        }
        cdp.codepage_hash[i] = NULL;
    }

    /* force a reload of the current codepage */
    cdp.curr_cp = NULL;
}
/*================================================================*/
//To be compatible to old version
void uop_init(void);
void uop_init(void)
{
	cdp_init_cdp();
}

bool set_codepage(armaddr_t pc);
bool set_codepage(armaddr_t pc)
{

	return cdp_set_cdp(pc);
}

void flush_all_codepages(void);
void flush_all_codepages(void)
{

	cdp_flush_all();
}
/*================================================================*/
/* end of cdp_uop.c */
