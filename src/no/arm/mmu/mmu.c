
/*------------------------------------
 * mmu.c
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

#include "../0ctr.h"
/*================================================================*/
#include "../1imp.h"
#include "mmu_reg.h"
#include "mmu_flag.h"
#include "mmu_page.h"

#include "../cpu/cpu.h"
#include "../cdp/cdp.h"
#include "../pfm/pfm.h"

/* NOTE: we will always check alignment if the flag is set, regardless of whether or not the mmu is present */

/* the memory interface the cpu should use for most of it's memory accesses */
enum mmu_access_type {
    INSTRUCTION_FETCH,
    DATA
};

enum mmu_permission_results {
    NO_ACCESS,
    READ_ONLY,
    READ_WRITE
};

enum mmu_domain_check_results {
    DOMAIN_FAULT,
    MANAGER,
    CLIENT,
};

#define TCACHE_PRESENT     0x1
#define TCACHE_WRITE       0x2
#define TCACHE_PRIVILEDGED 0x4

struct translation_cache_entry {
    unsigned int flags;
    armaddr_t vaddr;
    armaddr_t paddr_delta; // difference between the vaddr and paddr (only need an add to come up with the real address)
    unsigned long hostaddr_delta; // if nonzero, add this to the addr and cast to void * to get the address in the host's memory space
};

/*================================================================*/
#include "mmu_page.h"
#define NUM_TCACHE_ENTRIES 4096
#define TCACHE_PAGESIZE MMU_PAGESIZE

/* ARM mmu, specifically the arm926ejs variant */

struct mmu_state_struct {
    bool present;
    dword flags;
    armaddr_t translation_table;
    dword domain_access_control;
    dword fault_status;
    dword fault_address;

    bool fault;

    struct translation_cache_entry tcache_user_read[NUM_TCACHE_ENTRIES];
    struct translation_cache_entry tcache_user_write[NUM_TCACHE_ENTRIES];
    struct translation_cache_entry tcache_priviledged_read[NUM_TCACHE_ENTRIES];
    struct translation_cache_entry tcache_priviledged_write[NUM_TCACHE_ENTRIES];
};

static struct mmu_state_struct mmu; // defaults to off

/*================================================================*/
void mmu_init_mmu(int with_mmu);
void mmu_init_mmu(int with_mmu)
{
    os_memclr(&mmu, sizeof(mmu));

    if (with_mmu) {
        mmu.present = TRUE;
    }
}

/* translation cache code */
void mmu_invalidate_tcache(void);
void mmu_invalidate_tcache(void)
{
    int i;

    for (i = 0; i < NUM_TCACHE_ENTRIES; i++)  {
        mmu.tcache_user_read[i].flags = 0;
    }
    for (i = 0; i < NUM_TCACHE_ENTRIES; i++)  {
        mmu.tcache_priviledged_read[i].flags = 0;
    }
    for (i = 0; i < NUM_TCACHE_ENTRIES; i++)  {
        mmu.tcache_user_write[i].flags = 0;
    }
    for (i = 0; i < NUM_TCACHE_ENTRIES; i++)  {
        mmu.tcache_priviledged_write[i].flags = 0;
    }
}

dword mmu_set_flags(dword flags);
dword mmu_set_flags(dword flags)
{
    dword oldflags = mmu.flags;

    if (flags != oldflags) {
        MMU_TRACE(5, "mmu_set_flags: flags 0x%08x\n", flags);
        mmu.flags = flags;

        /* it may have changed S or R or mmu enable bit, flush our translation cache */
        mmu_invalidate_tcache();
    }

    return oldflags;
}

dword mmu_get_flags(void);
dword mmu_get_flags(void)
{
    MMU_TRACE(5, "mmu_get_flags: flags 0x%08x\n", mmu.flags);
    return mmu.flags;
}

void mmu_set_register(enum mmu_registers reg, dword val);
void mmu_set_register(enum mmu_registers reg, dword val)
{
    MMU_TRACE(5, "mmu_set_register: reg %d, val 0x%08x\n", reg, val);
    switch (reg) {
        case MMU_TRANS_TABLE_REG:
            mmu.translation_table = val;
            mmu_invalidate_tcache(); // TLB flush
            break;
        case MMU_DOMAIN_ACCESS_CONTROL_REG:
            mmu.domain_access_control = val;
            mmu_invalidate_tcache(); // TLB flush
            break;
        case MMU_FAULT_STATUS_REG:
            mmu.fault_status = val;
            break;
        case MMU_FAULT_ADDRESS_REG:
            mmu.fault_address = val;
            break;
    }
}

dword mmu_get_register(enum mmu_registers reg);
dword mmu_get_register(enum mmu_registers reg)
{
    MMU_TRACE(5, "mmu_get_register: reg %d\n", reg);
    switch (reg) {
        case MMU_TRANS_TABLE_REG:
            return mmu.translation_table;
        case MMU_DOMAIN_ACCESS_CONTROL_REG:
            return mmu.domain_access_control;
        case MMU_FAULT_STATUS_REG:
            return mmu.fault_status;
        case MMU_FAULT_ADDRESS_REG:
            return mmu.fault_address;
    }
    return 0;
}


/*================================================================*/
static inline int tcache_hash(armaddr_t vaddr)
{
    return (vaddr / TCACHE_PAGESIZE) % NUM_TCACHE_ENTRIES;
}

static inline struct translation_cache_entry *lookup_tcache_entry(armaddr_t vaddr, bool write, bool priviledged);
static inline struct translation_cache_entry *lookup_tcache_entry(armaddr_t vaddr, bool write, bool priviledged)
{
    struct translation_cache_entry *tcache;

    /* This inefficient looking switch of the tcache bank to search in is actually
     * somewhat faster than trying to calculate the bank dynamically.
     * In every case this routine is emitted, the write boolean is hard coded, so half
     * of these tests go away immediately, and the other test is pretty easy to make
     */
    if (write) {
        if (priviledged) {
            tcache = mmu.tcache_priviledged_write;
        } else {
            tcache = mmu.tcache_user_write;
        }
    } else {
        if (priviledged) {
            tcache = mmu.tcache_priviledged_read;
        } else {
            tcache = mmu.tcache_user_read;
        }
    }

    return &tcache[tcache_hash(vaddr)];
}

static void add_tcache_entry(armaddr_t vaddr, armaddr_t paddr, bool write, bool priviledged);
static void add_tcache_entry(armaddr_t vaddr, armaddr_t paddr, bool write, bool priviledged)
{
    struct translation_cache_entry *ent;
    void *host_ptr;

    /* fill out the entry */
    ent = lookup_tcache_entry(vaddr, write, priviledged);
    ent->vaddr = vaddr;

    /* ask the sys layer if we can get a direct pointer */
    host_ptr = sys_get_mem_ptr(paddr);
    if (host_ptr != NULL)
        ent->hostaddr_delta = (unsigned long)host_ptr - vaddr; // bit of pointer math here to speed up the eventual translation
    else
        ent->hostaddr_delta = 0;
    ent->paddr_delta = paddr - vaddr;
    ent->flags = 0;
    if (write)
        ent->flags |= TCACHE_WRITE;
    if (priviledged)
        ent->flags |= TCACHE_PRIVILEDGED;
    ent->flags |= TCACHE_PRESENT;

//  printf("add_tcache_entry: vaddr 0x%x paddr 0x%x write %d priviledged %d hostaddr_delta 0x%x paddr_delta 0x%x\n",
//      vaddr, paddr, write, priviledged, ent->hostaddr_delta, ent->paddr_delta);
}

static enum mmu_domain_check_results mmu_domain_check(int domain);
static enum mmu_domain_check_results mmu_domain_check(int domain)
{
    /* check domain permissions */
    /* table 3-4: Domain Access Values, page B3-17 */
    switch ((mmu.domain_access_control >> (domain * 2)) & 3) {
        default:
            case 3: // manager mode, all accesses are allowed
                    return MANAGER;
        case 1: // client mode, keep checking
            return CLIENT;
        case 0: // no access
        case 2: // reserved
            return DOMAIN_FAULT;
    }
}

/* full permission check */
static enum mmu_permission_results mmu_permission_check(int AP, int SR, bool priviledged);
static enum mmu_permission_results mmu_permission_check(int AP, int SR, bool priviledged)
{
    /* two different sets of logic based on priviledged or not */
    /* table 3-3: MMU access permissions, page B3-16 */
    if (priviledged) {
        if (AP == 0) {
            switch (SR) {
                case 0:
                        return NO_ACCESS;
                case 1:
                    return READ_ONLY;
                case 2:
                    return READ_ONLY;
                case 3:
                    return NO_ACCESS;
                default:
                    return NO_ACCESS;
            }
        } else {
            // full read/write for anything other than AP == 0
            return READ_WRITE;
        }
    } else { // user
        switch (AP) {
            case 0: {
                switch (SR) {
                    case 0:
                        return NO_ACCESS;
                    case 1:
                        return NO_ACCESS;
                    case 2:
                        return READ_ONLY;
                    case 3:
                    default:
                        return NO_ACCESS;
                }
            }
            break;
            case 1:
                return NO_ACCESS;
            case 2:
                return READ_ONLY;
            case 3:
                return READ_WRITE;
            default:
                return NO_ACCESS;
        }
    }
}

static void mmu_signal_fault(int status, int domain, armaddr_t address, enum mmu_access_type type);
static void mmu_signal_fault(int status, int domain, armaddr_t address, enum mmu_access_type type)
{
    mmu.fault_status = status | (domain << 4);
    mmu.fault_address = address;
    if (type == INSTRUCTION_FETCH)
        signal_prefetch_abort(address);
    else
        signal_data_abort(address);
    mmu.fault = TRUE;
}

static void mmu_2nd_level_translate(unsigned int ptable_entry, armaddr_t address, armaddr_t *translated_address, enum mmu_access_type type, bool write, bool priviledged, int domain);
static void mmu_2nd_level_translate(unsigned int ptable_entry, armaddr_t address, armaddr_t *translated_address, enum mmu_access_type type, bool write, bool priviledged, int domain)
{
    int subpage = 0;

    MMU_TRACE(7, "\t2nd level translate: ptable_entry 0x%08x\n", ptable_entry);

    switch (ptable_entry & 0x3) {
        case 1: // large page (64KB)
            *translated_address = BITS(ptable_entry, 31, 16) | BITS(address, 15, 0);

            /* figure out which subpage we are */
            subpage = BITS_SHIFT(address, 15, 14);
            break;
        case 2: // small page (4KB)
            *translated_address = BITS(ptable_entry, 31, 12) | BITS(address, 11, 0);

            /* figure out which subpage we are */
            subpage = BITS_SHIFT(address, 11, 10);
            break;
        case 3: // tiny page (1KB)
            *translated_address = BITS(ptable_entry, 31, 10) | BITS(address, 9, 0);

            /* there's only one subpage in on a tiny page descriptor */
            subpage = 0;
            break;
        case 0: // not present
            /* page translation fault */
            mmu_signal_fault(0x7, domain, address, type);
            return;
    }

    /* domain check */
    enum mmu_domain_check_results domain_check = mmu_domain_check(domain);
    if (domain_check == DOMAIN_FAULT) {
        /* page domain fault */
        mmu_signal_fault(0xb, domain, address, type);
        return;
    } else if (domain_check == CLIENT) {
        /* load the appropriate AP bits */
        int AP = (ptable_entry >> (subpage * 2 + 4)) & 0x3;

        /* do perm check on AP bits */
        enum mmu_permission_results allowed_perms = mmu_permission_check(AP, BITS_SHIFT(mmu.flags, 9, 8), priviledged);
        if (allowed_perms == NO_ACCESS || (allowed_perms == READ_ONLY && write)) {
            /* page permission fault */
            mmu_signal_fault(0xf, domain, address, type);
            return;
        }
    } else { // domain_check == MANAGER

    }

    /* add a translation entry */
    add_tcache_entry(address & ~(TCACHE_PAGESIZE-1), *translated_address & ~(TCACHE_PAGESIZE-1), write, priviledged);
}

#include "mmu_flag.h"
static armaddr_t mmu_slow_translate(armaddr_t address, enum mmu_access_type type, bool write, bool priviledged);
static armaddr_t mmu_slow_translate(armaddr_t address, enum mmu_access_type type, bool write, bool priviledged)
{
    armaddr_t translated_addr;
    unsigned int ttable_entry;
    unsigned int ptable_entry;
    int domain;

    mmu_inc_perf_counter(MMU_SLOW_TRANSLATE);

    if (!mmu.present || !(mmu.flags & MMU_ENABLED_FLAG)) {
        /* no mmu? create a identity translation cache entry */
        armaddr_t aligned_address = address & ~(TCACHE_PAGESIZE-1);
        add_tcache_entry(aligned_address, aligned_address, write, priviledged);
        return address;
    }

    // used as an extended way to signal a translation fault from this and lower routines to the caller
    mmu.fault = FALSE;

    /* read in the translation table entry */
    ttable_entry = bus_read_u32(mmu.translation_table + (address >> 20) * 4);

    MMU_TRACE(7, "\tttable_entry 0x%08x\n", ttable_entry);

    /* get the domain bits */
    domain = BITS_SHIFT(ttable_entry, 8, 5);

    /* do something based off the first level descriptor type */
    switch (ttable_entry & 0x3) {
        case 2: { // section
            /* domain check */
            enum mmu_domain_check_results domain_check = mmu_domain_check(domain);
            if (domain_check == DOMAIN_FAULT) {
                /* page domain fault */
                mmu_signal_fault(0x9, domain, address, type);
                return 0;
            } else if (domain_check == CLIENT) {
                /* permission check */
                int AP = BITS_SHIFT(ttable_entry, 11, 10);

                enum mmu_permission_results allowed_perms = mmu_permission_check(AP, BITS_SHIFT(mmu.flags, 9, 8), priviledged);
                if (allowed_perms == NO_ACCESS || (allowed_perms == READ_ONLY && write)) {
                    /* section permission fault */
                    mmu_signal_fault(0xd, domain, address, type);
                    return 0;
                }
            }

            /* we have the address */
            translated_addr = BITS(ttable_entry, 31, 20) | BITS(address, 19, 0);
            MMU_TRACE(7, "\tsection, translated_addr 0x%08x\n", translated_addr);

            /* add a translation entry */
            add_tcache_entry(address & ~(TCACHE_PAGESIZE-1), translated_addr & ~(TCACHE_PAGESIZE-1), write, priviledged);

            break;
        }
        case 1: // coarse page table
            ptable_entry = bus_read_u32(BITS(ttable_entry, 31, 10));

            /* do a second level translation */
            mmu_2nd_level_translate(ptable_entry, address, &translated_addr, type, write, priviledged, domain);
            break;
        case 3: // fine page table
            ptable_entry = bus_read_u32(BITS(ttable_entry, 31, 12));

            /* do a second level translation */
            mmu_2nd_level_translate(ptable_entry, address, &translated_addr, type, write, priviledged, domain);
            break;
        case 0:
            /* section translation fault */
            mmu_signal_fault(0x5, 0, address, type);
            return 0;
    }

    return translated_addr;
}

static inline struct translation_cache_entry *mmu_tcache_lookup(armaddr_t address, bool write, bool priviledged);
static inline struct translation_cache_entry *mmu_tcache_lookup(armaddr_t address, bool write, bool priviledged)
{
    struct translation_cache_entry *tcache_ent;

    /* do a fast lookup */
    tcache_ent = lookup_tcache_entry(address, write, priviledged);
    if (likely(tcache_ent->flags & TCACHE_PRESENT)) {
        armaddr_t vaddr_page = address & ~(TCACHE_PAGESIZE-1);
        if (likely(tcache_ent->vaddr == vaddr_page)) {
            /*
             * NOTE: permissions are implicitly checked by the fact that the entry exists
             * at all. The entry was added after a full permission check was performed
             * and if any of the global settings changed that may effect permissions
             * the entire cache was wiped
             */

            /* we have the entry */
            return tcache_ent;
        }
    }
    return NULL;
}

/*================================================================*/
/* instruction fetches */
bool mmu_read_instruction_dword(armaddr_t address, dword *data, bool priviledged);
bool mmu_read_instruction_dword(armaddr_t address, dword *data, bool priviledged)
{
    struct translation_cache_entry *tcache_ent;

    mmu_inc_perf_counter(MMU_INS_FETCH);

    /* do a translation lookup */
    tcache_ent = mmu_tcache_lookup(address, FALSE, priviledged);
    if (tcache_ent) {
        if (tcache_ent->hostaddr_delta != 0) {
            /* fast path, can read directly from host memory */
            mmu_inc_perf_counter(MMU_FASTPATH);
            *data = READ_MEM_DWORD((void *)(address + tcache_ent->hostaddr_delta));
            return FALSE;
        } else {
            /* slow path, must call into system layer to get memory */
            mmu_inc_perf_counter(MMU_SLOWPATH);
            *data = bus_read_u32(address);
            return FALSE;
        }
    }

    /* do a slow lookup which will add a translation cache entry for the next time */
    address = mmu_slow_translate(address, INSTRUCTION_FETCH, FALSE, priviledged);
    if (mmu.fault)
        return TRUE;

    *data = bus_read_u32(address);
    return FALSE;
}


bool mmu_read_instruction_word(armaddr_t address, word *data, bool priviledged);
bool mmu_read_instruction_word(armaddr_t address, word *data, bool priviledged)
{
    struct translation_cache_entry *tcache_ent;

    mmu_inc_perf_counter(MMU_INS_FETCH);

    /* do a translation lookup */
    tcache_ent = mmu_tcache_lookup(address, FALSE, priviledged);
    if (tcache_ent) {
        if (tcache_ent->hostaddr_delta != 0) {
            /* fast path, can read directly from host memory */
            mmu_inc_perf_counter(MMU_FASTPATH);
            *data = READ_MEM_WORD((void *)(address + tcache_ent->hostaddr_delta));
            return FALSE;
        } else {
            /* slow path, must call into system layer to get memory */
            mmu_inc_perf_counter(MMU_SLOWPATH);
            *data = bus_read_u16(address + tcache_ent->paddr_delta);
            return FALSE;
        }
    }

    /* do a slow lookup which will add a translation cache entry for the next time */
    address = mmu_slow_translate(address, INSTRUCTION_FETCH, FALSE, priviledged);
    if (mmu.fault)
        return TRUE;

    *data = bus_read_u16(address);
    return FALSE;
}

/*================================================================*/
/* regular memory fetches */

bool mmu_read_mem_dword(armaddr_t address, dword *data);
bool mmu_read_mem_dword(armaddr_t address, dword *data)
{
    MMU_TRACE(10, "mmu_read_mem_dword: addr 0x%x, data 0x%x\n", address, data);

    mmu_inc_perf_counter(MMU_READ);

    // alignment check
    if (unlikely(address & 3)) {
        if (mmu.flags & MMU_ALIGNMENT_FAULT_FLAG) {
            mmu.fault_status = 0x1;
            mmu.fault_address = address;
            signal_data_abort(address);
            return TRUE;
        }
    }

    /* do a translation lookup */
    bool priviledged = arm_in_priviledged();
    struct translation_cache_entry *tcache_ent = mmu_tcache_lookup(address, FALSE, priviledged);
    if (likely(tcache_ent)) {
        if (likely(tcache_ent->hostaddr_delta != 0)) {
            /* fast path, can read directly from host memory */
            mmu_inc_perf_counter(MMU_FASTPATH);
            *data = READ_MEM_DWORD((void *)(address + tcache_ent->hostaddr_delta));
            return FALSE;
        } else {
            /* slow path, must call into system layer to get memory */
            mmu_inc_perf_counter(MMU_SLOWPATH);
            *data = bus_read_u32(address + tcache_ent->paddr_delta);
            return FALSE;
        }
    }

    /* do a slow lookup which will add a translation cache entry for the next time */
    address = mmu_slow_translate(address, DATA, FALSE, priviledged);
    if (mmu.fault)
        return TRUE;

    *data = bus_read_u32(address);
    return FALSE;
}

bool mmu_read_mem_word(armaddr_t address, word *data);
bool mmu_read_mem_word(armaddr_t address, word *data)
{
    MMU_TRACE(10, "mmu_read_mem_word: addr 0x%x, data 0x%x\n", address, data);

    mmu_inc_perf_counter(MMU_READ);

    // alignment check
    if (address & 1) {
        if (mmu.flags & MMU_ALIGNMENT_FAULT_FLAG) {
            mmu.fault_status = 0x1;
            mmu.fault_address = address;
            signal_data_abort(address);
            return TRUE;
        }
    }

    /* do a translation lookup */
    bool priviledged = arm_in_priviledged();
    struct translation_cache_entry *tcache_ent = mmu_tcache_lookup(address, FALSE, priviledged);
    if (tcache_ent) {
        if (tcache_ent->hostaddr_delta != 0) {
            /* fast path, can read directly from host memory */
            mmu_inc_perf_counter(MMU_FASTPATH);
            *data = READ_MEM_WORD((void *)(address + tcache_ent->hostaddr_delta));
            return FALSE;
        } else {
            /* slow path, must call into system layer to get memory */
            mmu_inc_perf_counter(MMU_SLOWPATH);
            *data = bus_read_u16(address + tcache_ent->paddr_delta);
            return FALSE;
        }
    }

    /* do a slow lookup which will add a translation cache entry for the next time */
    address = mmu_slow_translate(address, DATA, FALSE, priviledged);
    if (mmu.fault)
        return TRUE;

    *data = bus_read_u16(address);
    return FALSE;

}

bool mmu_read_mem_byte(armaddr_t address, byte *data);
bool mmu_read_mem_byte(armaddr_t address, byte *data)
{
    MMU_TRACE(10, "mmu_read_mem_byte: addr 0x%x, data 0x%x\n", address, data);

    mmu_inc_perf_counter(MMU_READ);

    /* do a translation lookup */
    bool priviledged = arm_in_priviledged();
    struct translation_cache_entry *tcache_ent = mmu_tcache_lookup(address, FALSE, priviledged);
    if (tcache_ent) {
        if (tcache_ent->hostaddr_delta != 0) {
            /* fast path, can read directly from host memory */
            mmu_inc_perf_counter(MMU_FASTPATH);
            *data = READ_MEM_BYTE((void *)(address + tcache_ent->hostaddr_delta));
            return FALSE;
        } else {
            /* slow path, must call into system layer to get memory */
            mmu_inc_perf_counter(MMU_SLOWPATH);
            *data = bus_read_u8(address + tcache_ent->paddr_delta);
            return FALSE;
        }
    }

    /* do a slow lookup which will add a translation cache entry for the next time */
    address = mmu_slow_translate(address, DATA, FALSE, priviledged);
    if (mmu.fault)
        return TRUE;

    *data = bus_read_u8(address);
    return FALSE;
}

/*================================================================*/
/* regular memory writes */

bool mmu_write_mem_dword(armaddr_t address, dword data);
bool mmu_write_mem_dword(armaddr_t address, dword data)
{
    MMU_TRACE(10, "mmu_write_mem_dword: addr 0x%x, data 0x%x\n", address, data);

    mmu_inc_perf_counter(MMU_WRITE);

    // alignment check
    if (address & 3) {
        if (mmu.flags & MMU_ALIGNMENT_FAULT_FLAG) {
            mmu.fault_status = 0x1;
            mmu.fault_address = address;
            signal_data_abort(address);
            return TRUE;
        }
    }

    /* do a translation lookup */
    bool priviledged = arm_in_priviledged();
    struct translation_cache_entry *tcache_ent = mmu_tcache_lookup(address, TRUE, priviledged);
    if (tcache_ent) {
        if (tcache_ent->hostaddr_delta != 0) {
            /* fast path, can read directly from host memory */
            mmu_inc_perf_counter(MMU_FASTPATH);
            WRITE_MEM_DWORD((void *)(address + tcache_ent->hostaddr_delta), data);
            return FALSE;
        } else {
            /* slow path, must call into system layer to get memory */
            mmu_inc_perf_counter(MMU_SLOWPATH);
            bus_write_u32(address + tcache_ent->paddr_delta, data);
            return FALSE;
        }
    }

    /* do a slow lookup which will add a translation cache entry for the next time */
    address = mmu_slow_translate(address, DATA, TRUE, priviledged);
    if (mmu.fault)
        return TRUE;

    bus_write_u32(address, data);
    return FALSE;
}

bool mmu_write_mem_word(armaddr_t address, word data);
bool mmu_write_mem_word(armaddr_t address, word data)
{
    MMU_TRACE(10, "mmu_write_mem_word: addr 0x%x, data 0x%x\n", address, data);

    mmu_inc_perf_counter(MMU_WRITE);

    // alignment check
    if (address & 1) {
        if (mmu.flags & MMU_ALIGNMENT_FAULT_FLAG) {
            mmu.fault_status = 0x1;
            mmu.fault_address = address;
            signal_data_abort(address);
            return TRUE;
        }
    }

    /* do a translation lookup */
    bool priviledged = arm_in_priviledged();
    struct translation_cache_entry *tcache_ent = mmu_tcache_lookup(address, TRUE, priviledged);
    if (tcache_ent) {
        if (tcache_ent->hostaddr_delta != 0) {
            /* fast path, can read directly from host memory */
            mmu_inc_perf_counter(MMU_FASTPATH);
            WRITE_MEM_WORD((void *)(address + tcache_ent->hostaddr_delta), data);
            return FALSE;
        } else {
            /* slow path, must call into system layer to get memory */
            mmu_inc_perf_counter(MMU_SLOWPATH);
            bus_write_u16(address + tcache_ent->paddr_delta, data);
            return FALSE;
        }
    }

    /* do a slow lookup which will add a translation cache entry for the next time */
    address = mmu_slow_translate(address, DATA, TRUE, priviledged);
    if (mmu.fault)
        return TRUE;

    bus_write_u16(address, data);
    return FALSE;
}

bool mmu_write_mem_byte(armaddr_t address, byte data);
bool mmu_write_mem_byte(armaddr_t address, byte data)
{
    MMU_TRACE(10, "mmu_write_mem_byte: addr 0x%x, data 0x%x\n", address, data);

    mmu_inc_perf_counter(MMU_WRITE);

    /* do a translation lookup */
    bool priviledged = arm_in_priviledged();
    struct translation_cache_entry *tcache_ent = mmu_tcache_lookup(address, TRUE, priviledged);
    if (tcache_ent) {
        if (tcache_ent->hostaddr_delta != 0) {
            /* fast path, can read directly from host memory */
            mmu_inc_perf_counter(MMU_FASTPATH);
            WRITE_MEM_BYTE((void *)(address + tcache_ent->hostaddr_delta), data);
            return FALSE;
        } else {
            /* slow path, must call into system layer to get memory */
            mmu_inc_perf_counter(MMU_SLOWPATH);
            bus_write_u8(address + tcache_ent->paddr_delta, data);
            return FALSE;
        }
    }

    /* do a slow lookup which will add a translation cache entry for the next time */
    address = mmu_slow_translate(address, DATA, TRUE, priviledged);
    if (mmu.fault)
        return TRUE;

    bus_write_u8(address, data);
    return FALSE;
}

/*================================================================*/
//To be compatible to old version
void mmu_init(int with_mmu);
void mmu_init(int with_mmu)
{
	mmu_init_mmu(with_mmu);
}

/*================================================================*/
/* end of mmu.c */
