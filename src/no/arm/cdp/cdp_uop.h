
/*------------------------------------
 * uop_cdp.h
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

#include "../0ctr.h"

#ifndef NO_ARM_CDP_UOP_H_
#define NO_ARM_CDP_UOP_H_
/*================================================================*/
#include "../1imp.h"

#include "../uop/uop_dec.h"
/* a page of uops at a time */
struct uop_codepage {
    struct uop_codepage *next;
    armaddr_t address;

    bool thumb; /* arm or thumb */

    /*
     * how much to push the pc up by at every instruction.
     * also pc_inc * 2 is how much r15 will be ahead of the current instruction.
     */
    int pc_inc;
    int pc_shift; // number of bits the real pc should be shifted to get to the codepage index (2 for arm, 1 for thumb)

    struct uop ops[0]; /* we will allocate a different amount of space if it's arm or thumb */
};

#define CODEPAGE_HASHSIZE 1024

#define NUM_CODEPAGE_INS_ARM    (MMU_PAGESIZE / 4)
#define NUM_CODEPAGE_INS_THUMB  (MMU_PAGESIZE / 2)

/*================================================================*/
struct cdp_struct {
    // cache of uop codepages
    struct uop_codepage *curr_cp;
    struct uop_codepage *codepage_hash[CODEPAGE_HASHSIZE];

    // free list of codepage structures
    struct uop_codepage *free_cp_arm;
    struct uop_codepage *free_cp_thumb;
};

//Just for easily to change code.   cpu.curr_cp -> cdp.curr_cp
extern struct cdp_struct cdp;

#define PC_TO_CPPC(pc) &cdp.curr_cp->ops[((pc) % MMU_PAGESIZE) >> (cdp.curr_cp->pc_shift)];
/*================================================================*/
#endif 
/* end of uop_cdp.h */
