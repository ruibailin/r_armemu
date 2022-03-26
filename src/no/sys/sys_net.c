
/*------------------------------------
 * sys_net.c
 * Create:  2021-02-07
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

#define PACKET_LEN 2048
#define PACKET_QUEUE_LEN 32 /* must be power of 2 */

static struct network {
    int head;
    int tail;

    u32 out_packet_len;
    uint8_t out_packet[PACKET_LEN];
    u32 in_packet_len[PACKET_QUEUE_LEN];
    uint8_t in_packet[PACKET_QUEUE_LEN][PACKET_LEN];

    int fd;
} *network;

/*================================================================*/
static dword buffer_read(const void *_ptr, u32 offset, int size);
static dword buffer_read(const void *_ptr, u32 offset, int size)
{
    const uint8_t *ptr;
    ptr = (const uint8_t *)_ptr;

    switch (size) {
        default:
        case 1:
            return ptr[offset];
            break;
        case 2:
            return *(uint16_t *)&ptr[offset];
            break;
        case 4:
            return *(uint32_t *)&ptr[offset];
            break;
    }
}

static dword buffer_write(void *_ptr, u32 offset, dword data, int size);
static dword buffer_write(void *_ptr, u32 offset, dword data, int size)
{
    uint8_t *ptr;
    ptr = (uint8_t *)_ptr;

    switch (size) {
        default:
        case 1:
            ptr[offset] = data;
            break;
        case 2:
            *(uint16_t *)&ptr[offset] = data;
            break;
        case 4:
            *(uint32_t *)&ptr[offset] = data;
            break;
    }

    return data;
}

/*================================================================*/
#include "sys_pic.h"

#define NET_BUF_LEN 2048
#define NET_IN_BUF_COUNT 32

#define NET_HEAD    (NET_REGS_BASE + 0)     /* current next buffer the hardware will write to */
#define NET_TAIL    (NET_REGS_BASE + 4)     /* currently selected input buffer */
#define NET_SEND    (NET_REGS_BASE + 8)     /* writes to this register sends whatever is in the out buf */
#define NET_SEND_LEN (NET_REGS_BASE + 12)   /* length of packet to send */
#define NET_OUT_BUF (NET_REGS_BASE + NET_BUF_LEN)

#define NET_IN_BUF_LEN (NET_REGS_BASE + 16) /* length of the currently selected in buffer, via tail register */
#define NET_IN_BUF  (NET_REGS_BASE + NET_BUF_LEN*2)

static dword network_regs_get_put(armaddr_t address, dword data, int size, int put);
static dword network_regs_get_put(armaddr_t address, dword data, int size, int put)
{
    dword val;
    u32 offset;

    SYS_TRACE(5, "sys: network_regs_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    switch (address) {
        case NET_HEAD:
            /* head is read/only */
            val = network->head;
            break;
        case NET_TAIL:
            if (put) {
                network->tail = data % PACKET_QUEUE_LEN;
                if (network->head == network->tail) {
                    pic_deassert_level(INT_NET);
                }
            }
            val = network->tail;
            break;
        case NET_SEND:
            if (put) {
                os_write(network->fd, network->out_packet, network->out_packet_len);
            }
            val = 0;
            break;
        case NET_SEND_LEN:
            if (put) {
                network->out_packet_len = data % PACKET_LEN;
            }
            val = network->out_packet_len;
            break;
        case NET_IN_BUF_LEN:
            /* read/only */
            val = network->in_packet_len[network->tail];
            break;
        case NET_OUT_BUF...(NET_OUT_BUF + NET_BUF_LEN - 1):
            offset = address - NET_OUT_BUF;
            if (put) {
                val = buffer_write(network->out_packet, offset, data, size);
            } else {
                val = buffer_read(network->out_packet, offset, size);
            }
            break;
        case NET_IN_BUF...(NET_IN_BUF + NET_BUF_LEN - 1):
            offset = address - NET_IN_BUF;

            /* in buffers are read/only */
            val = buffer_read(network->in_packet[network->tail], offset, size);
            break;
        default:
            SYS_TRACE(0, "sys: unhandled network address 0x%08x\n", address);
            return 0;
    }

    return val;
}

/*================================================================*/
void network_receive_thread(void);
void network_receive_thread()
{
    u32 ret;
    ret = os_read(network->fd, network->in_packet[network->head], PACKET_LEN);
    if (ret > 0) {
        SYS_TRACE(2, "sys: got network data, size %d, head %d, tail %d\n", ret, network->head, network->tail);

        network->in_packet_len[network->head] = ret;

        network->head = (network->head + 1) % PACKET_QUEUE_LEN;
        pic_assert_level(INT_NET);
    }
}


void sys_ini_net();
void sys_ini_net()
{
	int host_mem_len;
	void *host_mem_addr;


    network = os_calloc(sizeof(*network), 1);

	host_mem_len = sizeof(network);
	host_mem_addr = network;
    // install the network register handlers
	bus_set_func(NET_REGS_BASE,&network_regs_get_put,NULL);
	bus_set_host_base_addr(NET_REGS_BASE,host_mem_addr);
	bus_bat_set_func(NET_REGS_BASE, host_mem_len);
}


void sys_del_net();
void sys_del_net()
{
	void *host_mem_addr;
	//	host_mem_addr = bus_get_mem_ptr(NET_REGS_BASE);		//network
	host_mem_addr = network;
	os_free(host_mem_addr);
}
/*================================================================*/
/* end of sys_net.c */
