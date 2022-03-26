
/*------------------------------------
 * sys_pic.c
 * Create:  2021-02-04
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
#if ARM_32_BUS
#include "../b32/2exp.h"
#else
#include "../b64/2exp.h"
#endif
#include "../util/2exp.h"

/*================================================================*/
static struct pic_t {

    uint32_t vector_mask;      // 1 if the interrupt is masked
    uint32_t vector_active;    // 1 if active

    uint32_t unmask;		//PIC_UNMASK_LATCH  (PIC_REGS_BASE + 8)
    uint32_t stat;			//PIC_STAT          (PIC_REGS_BASE + 12)
    uint32_t cur_bit;		//PIC_CURRENT_BIT   (PIC_REGS_BASE + 16)
    uint32_t cur_num;		//PIC_CURRENT_NUM   (PIC_REGS_BASE + 20)

    void *mutex;
    bool irq_active;
} pic;

/*================================================================*/
/* set cpu irq status based off of current interrupt controller inputs */
static void set_irq_status(void);
static void set_irq_status(void)
{
    if (pic.vector_active & ~pic.vector_mask) {
        if (!pic.irq_active)
            bus_raise_irq();
        pic.irq_active = TRUE;
    } else {
        if (pic.irq_active)
            bus_lower_irq();
        pic.irq_active = FALSE;
    }
}

#define PIC_MAX_INT 32
static int get_current_interrupt(void);
static int get_current_interrupt(void)
{
    int i;

    uint32_t ready_ints;

    ready_ints= pic.vector_active & (~pic.vector_mask);
    if (ready_ints == 0)
        return -1;

    for (i=0; i < PIC_MAX_INT; i++) {
        if (ready_ints & (1 << i))
            return i;
    }

    return -1;
}

/*================================================================*/
int pic_assert_level(int vector);
int pic_assert_level(int vector)
{
    if (vector < 0 || vector >= PIC_MAX_INT)
        return -1;

    SYS_TRACE(5, "sys: pic_assert_level %d\n", vector);

    pic.vector_active |= (1<<vector);
    set_irq_status();

    return 0;
}

int pic_deassert_level(int vector);
int pic_deassert_level(int vector)
{
    if (vector < 0 || vector >= PIC_MAX_INT)
        return -1;

    SYS_TRACE(5, "sys: pic_deassert_level %d\n", vector);

    pic.vector_active &= ~(1<<vector);
    set_irq_status();

    return 0;
}

/*================================================================*/
/* Current vector mask, read-only */
#define PIC_MASK          (PIC_REGS_BASE + 0)
/* Mask any of the 32 interrupt vectors by writing a 1 in the appropriate bit */
#define PIC_MASK_LATCH    (PIC_REGS_BASE + 4)
/* Unmask any of the 32 interrupt vectors by writing a 1 in the appropriate bit */
#define PIC_UNMASK_LATCH  (PIC_REGS_BASE + 8)
/* each bit corresponds to the current status of the interrupt line */
#define PIC_STAT          (PIC_REGS_BASE + 12)
/* one bit set for the highest priority non-masked active interrupt */
#define PIC_CURRENT_BIT   (PIC_REGS_BASE + 16)
/* holds the current interrupt number of the highest priority non-masked active interrupt,
 * or 0xffffffff if no interrupt is active
 */
#define PIC_CURRENT_NUM   (PIC_REGS_BASE + 20)

static dword pic_regs_get_put(armaddr_t address, dword data, int size, int put);
static dword pic_regs_get_put(armaddr_t address, dword data, int size, int put)
{
    dword val;

    SYS_TRACE(5, "sys: pic_regs_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    if (size < 4)
        return 0; /* only word accesses supported */

    switch (address) {
        /* read/write to the current interrupt mask */
        case PIC_MASK_LATCH: /* 1s are latched into the current mask */
            data |= pic.vector_mask;
            goto set_mask;
        case PIC_UNMASK_LATCH: /* 1s are latched as 0s in the current mask */
            data = pic.vector_mask & ~data;
set_mask:
        case PIC_MASK:
            if (put) {
                pic.vector_mask = data;
                val = 0;
                set_irq_status();
            } else {
                val = pic.vector_mask;
            }
            break;

        /* each bit corresponds to the current status of the interrupt line */
        case PIC_STAT:
            val = pic.vector_active;
            break;

        /* one bit set for the highest priority non-masked active interrupt */
        case PIC_CURRENT_BIT: {
            int current_int = get_current_interrupt();

            val = (current_int >= 0) ? (1 << current_int) : 0;
            break;
        }

        /* holds the current interrupt number, check PIC_CURRENT_BIT to see if something is pending */
        case PIC_CURRENT_NUM: {
            int current_int = get_current_interrupt();
            val = (current_int >= 0) ? (word)current_int : 0xffffffff;
            break;
        }

        default:
            val = 0;
    }

    return val;
}


/*================================================================*/
void sys_ini_pic(void);
void sys_ini_pic(void)
{
	int host_mem_len;
	void *host_mem_addr;
    os_memclr(&pic, sizeof(pic));

    //  pic.vector_mask = 0xffffffff; /* everything starts out masked */

	host_mem_len = sizeof(pic);
	host_mem_addr = &pic;
    // install the pic register handlers
	bus_set_func(PIC_REGS_BASE, pic_regs_get_put,NULL);
	bus_set_host_base_addr(PIC_REGS_BASE, host_mem_addr);
	bus_bat_set_func(PIC_REGS_BASE, host_mem_len);
}

void sys_del_pic(void);
void sys_del_pic(void)
{

}
/*================================================================*/

/* end of sys_pic.c */
