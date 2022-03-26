
/*------------------------------------
 * sys_dis.c
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
#if ARM_32_BUS
#include "../1imp.h"
#include "../b32/2exp.h"
#else
#include "../b64/2exp.h"
#endif
#include "../util/2exp.h"


#define DISPLAY_FRAMEBUFFER 	DISPLAY_BASE

#define DISPLAY_WIDTH     (DISPLAY_REGS_BASE + 0) // pixels width/height read/only
#define DISPLAY_HEIGHT    (DISPLAY_REGS_BASE + 4)
#define DISPLAY_BPP       (DISPLAY_REGS_BASE + 8) // bits per pixel (16/32)
/*================================================================*/
#define DEFAULT_SCREEN_X        640
#define DEFAULT_SCREEN_Y        480
#define DEFAULT_SCREEN_DEPTH    32

static struct display_t {


    // geometry
    uint32 screen_x;
    uint32 screen_y;
    uint32 screen_depth;
    uint32 screen_size;

    // dirty flag
    int dirty;

    // framebuffer backing store
    byte *fb;
    void *fbsurface;

    // SDL surface structure
    void *screen;

    void *thread_id;
} display;


/*================================================================*/
static dword display_regs_get_put(armaddr_t address, dword data, int size, int put);
static dword display_regs_get_put(armaddr_t address, dword data, int size, int put)
{
    dword ret;

    SYS_TRACE(5, "sys: display_regs_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    switch (address) {
        case DISPLAY_WIDTH:
            ret = display.screen_x;
            break;
        case DISPLAY_HEIGHT:
            ret = display.screen_y;
            break;
        case DISPLAY_BPP:
            ret = display.screen_depth;
            break;
        default:
            ret = 0;
    }


    return ret;
}

static dword display_get_put(armaddr_t address, dword data, int size, int put)
{
    byte *ptr;

    address -= DISPLAY_FRAMEBUFFER;

    if (unlikely(address > DISPLAY_SIZE))
        SYS_TRACE(0, "sys: display_get_put with invalid address 0x%08x\n", address);

    ptr = display.fb + address;

    switch (size) {
        case 4:
            if (put)
                WRITE_MEM_DWORD(ptr, data);
            else
                data = READ_MEM_DWORD(ptr);
            break;
        case 2:
            if (put)
                WRITE_MEM_WORD(ptr, data);
            else
                data = READ_MEM_WORD(ptr);
            break;
        case 1:
            if (put)
                WRITE_MEM_BYTE(ptr, data);
            else
                data = READ_MEM_BYTE(ptr);
            break;
    }

    if (put)
        atomic_or(&display.dirty, 1);

    SYS_TRACE(6, "sys: display_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    return data;
}

/*================================================================*/
//can be directly called, if don't want to use in a independent thread;
void sys_dis_proc_entry(void);
void sys_dis_proc_entry(void)
{
    dword *src;
    dword *dest;

    // is the surface dirty?
   if (atomic_set(&display.dirty, 0))
    {

        //os_printf("disppppp \n");
        // redraw the entire window
        os_sdl1_lock_surface();

        src = (dword *)display.fb;
        dest = (dword *)os_sdl1_get_surface_pixels();

        os_memcpy(dest, src, display.screen_size);

        os_sdl1_unlock_surface();
        os_sdl1_flip_surface();
    }

}


//if use SDL thread.
// main display loop
#include "sys_con.h"
static int display_thread_entry(void *args);
static int display_thread_entry(void *args)
{
	int i;
    for (;;) {
    	i=sys_con_thread_stop();
    	if(i)
    		break;
        os_sdl1_delay(20);

        sys_dis_proc_entry();
    }
    return 0;
}

static void test_display();
static void test_display()
{
	unsigned int i,j;
	byte *ptr;
	ptr = display.fb;
	for(i=0;i<display.screen_x;i++)
		for(j=0;j<display.screen_y;j++)
		{
			*ptr =(BYTE)j;
			ptr++;
			*ptr =(BYTE)j;
			ptr++;
			*ptr =(BYTE)j;
			ptr++;
			*ptr =0;
			ptr++;
		}
	display.dirty=1;
	sys_dis_proc_entry();
}
/*================================================================*/
void sys_ini_dis(void);
void sys_ini_dis(void)
{
	int host_mem_len;
	void *host_mem_add;

    // set up default geometry
    display.screen_x = DEFAULT_SCREEN_X;
    display.screen_y = DEFAULT_SCREEN_Y;
    display.screen_depth = DEFAULT_SCREEN_DEPTH;

    // calculate size
    display.screen_size = display.screen_x * display.screen_y * (display.screen_depth / 8);

    // create and register a memory range for the framebuffer
    display.fb = (byte *)os_calloc(display.screen_size, 1);

    host_mem_len = display.screen_size;
    host_mem_add = display.fb;
    bus_set_func(DISPLAY_BASE,display_get_put,NULL);
    bus_set_host_base_addr(DISPLAY_BASE, host_mem_add);
	bus_bat_set_func(DISPLAY_BASE, host_mem_len);

//    host_mem_len = DISPLAY_BPP+4-DISPLAY_REGS_BASE;
//    host_mem_add=os_malloc(host_mem_len);
	host_mem_len = sizeof(display);
	host_mem_add = &display;
	bus_set_func(DISPLAY_REGS_BASE,display_regs_get_put,NULL);
    bus_set_host_base_addr(DISPLAY_REGS_BASE, host_mem_add);
	bus_bat_set_func(DISPLAY_REGS_BASE, host_mem_len);

	os_sdl1_init_sdl();
	os_sdl1_init_sub_sys();
	os_sdl1_set_v_mod(display.screen_x, display.screen_y, display.screen_depth);
	os_sdl1_set_caption("c_soc","c_soc");
	os_sdl1_update_rect(0,0,0,0);

	test_display();
}

void sys_ini_dis_thread(void);
void sys_ini_dis_thread(void)
{
	display.thread_id=os_sdl1_create_thread(&display_thread_entry);
}

void sys_del_dis(void);
void sys_del_dis(void)
{
	void *host_mem_add;


	os_sdl1_quit_sub_sys();
	os_sdl1_quit_sdl();

//	host_mem_add = bus_get_mem_ptr(DISPLAY_REGS_BASE);
//	os_free(host_mem_add);
	host_mem_add = display.fb;
	os_free(host_mem_add);
}

void sys_del_dis_thread(void);
void sys_del_dis_thread(void)
{
	os_sdl1_destroy_thread(display.thread_id);
}
/*================================================================*/

/* end of sys_dis.c */
