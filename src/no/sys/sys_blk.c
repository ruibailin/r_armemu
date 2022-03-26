
/*------------------------------------
 * sys_blk.c
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


#define BDEV_CMD    	(BDEV_REGS_BASE + 0)    /* command */
#define BDEV_CMD_ADDR   (BDEV_REGS_BASE + 4)    /* address of next transfer, 32bit */
#define BDEV_CMD_OFF    (BDEV_REGS_BASE + 8)    /* offset of next transfer, 64bit */
#define BDEV_CMD_LEN    (BDEV_REGS_BASE + 16)   /* length of next transfer, 32bit */

#define BDEV_LEN    (BDEV_REGS_BASE + 20)   /* length of block device, 64bit */

/* BDEV_CMD bits */
#define BDEV_CMD_MASK   (0x3)
#define BDEV_CMD_NOP    (0)
#define BDEV_CMD_READ   (1)
#define BDEV_CMD_WRITE  (2)
#define BDEV_CMD_ERASE  (3)
#define BDEV_CMD_ERRSHIFT   16
#define BDEV_CMD_ERRMASK    (0xffff << BDEV_CMD_ERRSHIFT)
#define BDEV_CMD_ERR_NONE (0 << BDEV_CMD_ERRSHIFT)
#define BDEV_CMD_ERR_GENERAL (1 << BDEV_CMD_ERRSHIFT)
#define BDEV_CMD_ERR_BAD_OFFSET (2 << BDEV_CMD_ERRSHIFT)
/*================================================================*/
struct bdev_t {
    // pending command stuff
    uint32_t cmd;
    armaddr_t trans_addr;
    unsigned long trans_off;
    uint32_t trans_len;
    unsigned long length;
    // error codes
    uint32_t last_err;
    // used by mimic block device;
    int fd;
};

static struct bdev_t *bdev;

/*================================================================*/
static uint32_t bdev_read(armaddr_t address, unsigned long offset, uint32_t length);
static uint32_t bdev_read(armaddr_t address, unsigned long offset, uint32_t length)
{
    SYS_TRACE(1, "sys: bdev_read at 0x%08x, offset 0x%16llx, size %zd\n",
              address, offset, length);

    byte buf[4096];

    os_lseek_set(bdev->fd, offset);
    while (length > 0) {
        uint32_t tohandle = MIN(sizeof(buf), length);

        int err = os_read(bdev->fd, buf, tohandle);
        if (err != tohandle)
            return BDEV_CMD_ERR_GENERAL;

        uint32_t i;
        for (i = 0; i < tohandle / 4; i++) {
            bus_write_u32(address + i*4, *(dword *)(&buf[i * 4]));
        }

        for (; i < tohandle; i++)
        	bus_write_u8(address + i, *(dword *)(&buf[i]));

        length -= tohandle;
        address += tohandle;
    }

    return BDEV_CMD_ERR_NONE;
}

static uint32_t bdev_write(armaddr_t address, unsigned long offset, uint32_t length);
static uint32_t bdev_write(armaddr_t address, unsigned long offset, uint32_t length)
{
    SYS_TRACE(5, "sys: bdev_write at 0x%08x, offset 0x%16llx, size %zd\n",
              address, offset, length);

    byte buf[4096];

    os_lseek_set(bdev->fd, offset);
    while (length > 0) {
        uint32_t tohandle = MIN(sizeof(buf), length);

        uint32_t i;
        for (i = 0; i < tohandle / 4; i++) {
            *(dword *)(&buf[i * 4]) = bus_read_u32(address + i*4);
        }

        for (; i < tohandle; i++)
            *(dword *)(&buf[i]) = bus_read_u8(address + i);

       int err = os_write(bdev->fd, buf, tohandle);
        if (err != tohandle)
            return BDEV_CMD_ERR_GENERAL;

        length -= tohandle;
        address += tohandle;
    }

    return BDEV_CMD_ERR_NONE;
}

static uint32_t bdev_erase(unsigned long offset, uint32_t length);
static uint32_t bdev_erase(unsigned long offset, uint32_t length)
{
    SYS_TRACE(5, "sys: bdev_erase offset 0x%16llx, size %zd\n",
              offset, length);

    char buf[512];
    os_memclr(buf, sizeof(buf));

    os_lseek_set(bdev->fd, offset);
    while (length > 0) {
        uint32_t towrite = MIN(sizeof(buf), length);

        int err = os_write(bdev->fd, buf, towrite);
        if (err != 0)
            return BDEV_CMD_ERR_GENERAL;

        length -= towrite;
    }

    return BDEV_CMD_ERR_NONE;
}

static dword bdev_regs_get_put(armaddr_t address, dword data, int size, int put);
static dword bdev_regs_get_put(armaddr_t address, dword data, int size, int put)
{
    dword val;

    SYS_TRACE(5, "sys: bdev_regs_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    val = 0;
    switch (address) {
        case BDEV_CMD:
            if (put) {
                /* mask out the command portion of the write */
                data &= BDEV_CMD_MASK;

                switch (data) {
                    case BDEV_CMD_READ:
                        bdev->last_err = bdev_read(bdev->trans_addr, bdev->trans_off, bdev->trans_len);
                        break;
                    case BDEV_CMD_WRITE:
                        bdev->last_err = bdev_write(bdev->trans_addr, bdev->trans_off, bdev->trans_len);
                        break;
                    case BDEV_CMD_ERASE:
                        bdev->last_err = bdev_erase(bdev->trans_off, bdev->trans_len);
                        break;
                }
            } else {
                // read last error
                val = (bdev->last_err << BDEV_CMD_ERRSHIFT) | bdev->cmd;
            }
            break;
        case BDEV_CMD_ADDR:
            if (put) {
                bdev->trans_addr = data;
            } else {
                val = bdev->trans_addr;
            }
            break;
        case BDEV_CMD_OFF:
            if (put) {
                bdev->trans_off = (bdev->trans_off & 0xffffffff00000000ULL) | data;
            } else {
                val = (bdev->trans_off & 0xffffffff);
            }
            break;
        case BDEV_CMD_OFF + 4:
            if (put) {
                bdev->trans_off = (bdev->trans_off & 0xffffffff) | ((unsigned long)data << 32);
            } else {
                val = (bdev->trans_off >> 32);
            }
            break;
        case BDEV_CMD_LEN:
            if (put) {
                bdev->trans_len = data;
            } else {
                val = bdev->trans_len;
            }
            break;
        case BDEV_LEN:
            if (!put) {
                val = (bdev->length & 0xffffffff);
            }
            break;
        case BDEV_LEN + 4:
            if (!put) {
                val = (bdev->length >> 32);
            }
            break;

        default:
            SYS_TRACE(0, "sys: unhandled bdev address 0x%08x\n", address);
            return 0;
    }

    return val;
}


/*================================================================*/
extern const char *blk_dev_name;
void sys_ini_blk();
void sys_ini_blk()
{
	int host_mem_len;
	void *host_mem_addr;

    bdev = (struct bdev_t *)os_malloc(sizeof(*bdev));
    os_memclr(bdev,sizeof bdev);

    bdev->fd = os_open(blk_dev_name,2);

    if (bdev->fd == 0)
    {
        os_printf("Open bin file failed: %s, The reason is %s !\n", blk_dev_name,os_ferror_name());
        return;
    }

    bdev->length = os_get_file_len(bdev->fd);

//    host_mem_len = BDEV_LEN+8-BDEV_REGS_BASE;
//    host_mem_addr = os_malloc(host_mem_len);
	host_mem_len = sizeof(*bdev);
	host_mem_addr = bdev;

    bus_set_func(BDEV_REGS_BASE,bdev_regs_get_put,NULL);
    bus_set_host_base_addr(BDEV_REGS_BASE, host_mem_addr);
	bus_bat_set_func(BDEV_REGS_BASE,host_mem_len);

    SYS_TRACE(0, "sys: bdev fd %d, len %lld\n", bdev->fd, bdev->length);
}

void sys_del_blk();
void sys_del_blk()
{
	void *host_mem_addr;
//	host_mem_addr = bus_get_mem_ptr(BDEV_REGS_BASE);
	host_mem_addr = bdev;
	os_free(host_mem_addr);
}

/*================================================================*/

/* end of sys_blk.c */
