
/*------------------------------------
 * sys_con.c
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


#define KB_BUFSIZE 1024

static struct console_t {
    // XXX implement keyboard buffer
    int head;
    int tail;
    unsigned int keybuffer[KB_BUFSIZE];

    void *thread_id;
    int stop;
} console;

/*================================================================*/
#include "sys_pic.h"
#define IS_BUF_FULL (((console.tail - 1) % KB_BUFSIZE) == console.head)
#define IS_BUF_EMPTY (console.tail == console.head)
static void insert_key(int key);
static void insert_key(int key)
{
    /* for now, just stuff the raw sdl key into the buffer */
    if (!IS_BUF_FULL) {
        console.keybuffer[console.head] = key;
        console.head = (console.head + 1) % KB_BUFSIZE;
    }

    /* assert an interrupt */
    pic_assert_level(INT_KEYBOARD);
}

static unsigned int read_key(void)
{
    unsigned int key = 0;

    if (!IS_BUF_EMPTY) {
        key = console.keybuffer[console.tail];
        console.tail = (console.tail + 1) % KB_BUFSIZE;
    }

    /* if the buffer is now empty, deassert interrupt */
    if (IS_BUF_EMPTY)
        pic_deassert_level(INT_KEYBOARD);

    return key;
}

/*================================================================*/
#define KYBD_STAT         (CONSOLE_REGS_BASE + 0)
#define KYBD_DATA         (CONSOLE_REGS_BASE + 4)
static u32 console_regs_get_put(armaddr_t address, u32 data, int size, int put);
static u32 console_regs_get_put(armaddr_t address, u32 data, int size, int put)
{
    u32 val;

    SYS_TRACE(5, "sys: console_regs_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    if (put)
        return 0; /* can't write to the keyboard */

    switch (address) {
        case KYBD_STAT: /* status register */
            val = IS_BUF_EMPTY ? 0 : 1; /* we're not empty */
            break;
        case KYBD_DATA: /* keyboard data */
            val = read_key();
            break;
        default:
            SYS_TRACE(0, "unhandled keyboard address 0x%08x\n", address);
            return 0;
    }

    return val;
}

/*================================================================*/
enum {
    KEY_MOD_LSHIFT = 0x00010000,
    KEY_MOD_RSHIFT = 0x00020000,
    KEY_MOD_LCTRL  = 0x00040000,
    KEY_MOD_RCTRL  = 0x00080000,
    KEY_MOD_LALT   = 0x00100000,
    KEY_MOD_RALT   = 0x00200000,
    KEY_MOD_LMETA  = 0x00400000,
    KEY_MOD_RMETA  = 0x00800000,
    KEY_MOD_NUM    = 0x01000000,
    KEY_MOD_CAPS   = 0x02000000,
    KEY_MOD_MODE   = 0x04000000,
    KEY_MOD_UP     = 0x80000000,
    KEY_MOD_MASK   = 0xffff0000,
};

static void con_key_down(int key);
static void con_key_down(int key)
{
	os_printf("console_keydown: key 0x%x\n", key);

    insert_key(key);
}

static void con_key_up(int key);
static void con_key_up(int key)
{
	os_printf("console_keyup: key 0x%x\n", key);

    insert_key(key | KEY_MOD_UP);
}


/*================================================================*/
//can be directly called, if don't want to use in a independent thread;
int sys_con_proc_entry(void);
int sys_con_proc_entry(void)
{
	bool i;
	int key;
	os_sdl1_wait_event();
	i = os_sdl1_is_key_up();
	if(i)
	{
		key = os_sdl1_get_key_value();
		con_key_up(key);
//		return 0;
	}
	i = os_sdl1_is_key_down();
	if(i)
	{
		key = os_sdl1_get_key_value();
		con_key_down(key);
//		return 0;
	}
	i = os_sdl1_is_key_esc();
	console.stop = i;
	if(i)
		return 1;			//can exit main loop
	return 0;
}


//if use SDL thread.
// main console loop
static int console_thread_entry(void *args);
static int console_thread_entry(void *args)
{
    int quit = 0;
    while (!quit)
    {
    	quit = sys_con_proc_entry();
    }
	console.stop = 1;
    return 0;
}
/*================================================================*/
void sys_ini_con(void);
void sys_ini_con(void)
{
	int host_mem_len;
	void *host_mem_addr;

    os_memclr(&console, sizeof(console));

//    host_mem_len = KYBD_DATA+4-CONSOLE_REGS_BASE;
//    host_mem_addr=os_malloc(host_mem_len);
	host_mem_len = sizeof(console);
	host_mem_addr = &console;
    // install the console register handlers
	bus_set_func(CONSOLE_REGS_BASE,console_regs_get_put,NULL);
	bus_set_host_base_addr(CONSOLE_REGS_BASE,host_mem_addr);
	bus_bat_set_func(CONSOLE_REGS_BASE,host_mem_len);

}

void sys_ini_con_thread(void);
void sys_ini_con_thread(void)
{
	console.thread_id=os_sdl1_create_thread(&console_thread_entry);
}

int sys_con_thread_stop(void);
int sys_con_thread_stop(void)
{
	return console.stop;
}


void sys_del_con(void);
void sys_del_con(void)
{
//	void *host_mem_addr;
//	host_mem_addr = bus_get_mem_ptr(CONSOLE_REGS_BASE);
//	os_free(host_mem_addr);
}

void sys_del_con_thread(void);
void sys_del_con_thread(void)
{
	os_sdl1_destroy_thread(console.thread_id);
}
/*================================================================*/
/* end of sys_con.c */
