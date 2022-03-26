
/*------------------------------------
 * cdp.h
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

#ifndef NO_ARM_CDP_CDP_H_
#define NO_ARM_CDP_CDP_H_
/*================================================================*/
#include "cdp_uop.h"

/*================================================================*/
extern void cdp_init_cdp(void);
extern bool cdp_set_cdp(armaddr_t pc);
extern void cdp_flush_all(void);
/*================================================================*/
//To be compatible to old version
extern unsigned int codepage_hash(armaddr_t address, bool thumb);
extern void free_codepage(struct uop_codepage *cp);
extern struct uop_codepage *alloc_codepage_arm(void);
extern struct uop_codepage *alloc_codepage_thumb(void);
extern struct uop_codepage *lookup_codepage(armaddr_t pc, bool thumb);
extern bool load_codepage_arm(armaddr_t pc, armaddr_t cp_addr, bool priviledged, struct uop_codepage **_cp, int *last_ins_index);
extern bool load_codepage_thumb(armaddr_t pc, armaddr_t cp_addr, bool priviledged, struct uop_codepage **_cp, int *last_ins_index);
extern bool load_codepage(armaddr_t pc, bool thumb, bool priviledged, struct uop_codepage **_cp);

extern void uop_init(void);
extern bool set_codepage(armaddr_t pc);
extern void flush_all_codepages(void);
/*================================================================*/
#endif
/* end of cdp.h */
