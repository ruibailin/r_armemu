
/*------------------------------------
 * mmu.h
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

#ifndef NO_ARM_MMU_MMU_H_
#define NO_ARM_MMU_MMU_H_
/*================================================================*/
#include "mmu_reg.h"
#include "mmu_flag.h"
#include "mmu_page.h"

extern void mmu_init_mmu(int with_mmu);
/*================================================================*/
extern bool mmu_read_instruction_dword(addr_t address, dword *data, bool priviledged);
extern bool mmu_read_instruction_word(addr_t address, word *data, bool priviledged);
/*================================================================*/
extern bool mmu_read_mem_dword(addr_t address, dword *data);
extern bool mmu_read_mem_word(addr_t address, word *data);
extern bool mmu_read_mem_byte(addr_t address, byte *data);
extern bool mmu_write_mem_dword(addr_t address, dword data);
extern bool mmu_write_mem_word(addr_t address, word data);
extern bool mmu_write_mem_byte(addr_t address, byte data);
/*================================================================*/
extern dword mmu_set_flags(dword flags);
extern dword mmu_get_flags(void);
/*================================================================*/
extern void mmu_set_register(reg_t reg, dword val);
extern dword mmu_get_register(reg_t reg);
extern void mmu_invalidate_tcache(void);

/*================================================================*/
extern void mmu_init(int with_mmu);

/*
extern bool mmu_read_instruction_word(armaddr_t address, word *data, bool priviledged);
extern bool mmu_read_instruction_halfword(armaddr_t address, halfword *data, bool priviledged);
extern bool mmu_read_mem_word(armaddr_t address, word *data);
extern bool mmu_read_mem_halfword(armaddr_t address, halfword *data);
extern bool mmu_write_mem_word(armaddr_t address, word data);
extern bool mmu_write_mem_halfword(armaddr_t address, halfword data);
*/
/*================================================================*/
#endif 
/* end of mmu.h */
