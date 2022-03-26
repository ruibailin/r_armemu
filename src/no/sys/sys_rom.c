
/*------------------------------------
 * sys_rom.c
 * Create:  2021-02-03
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

#if _USE_HOST_FILE_AS_MEM_
#define	ROM_FILE_NUM  ROM_SIZE/MEM_SLOT_SIZE
static char *rom_file_name[ROM_FILE_NUM];
static void *rom_fp[ROM_FILE_NUM];

/*================================================================*/
static void int_to_str(char *s, int i)
{
	int j;
	j=8;
	for(;;)
	{
		if(i==0)
			break;
		s[j]=i%10+'0';
		j--;
		i=i/10;
	}
}

static void rom_open_file(void);
static void rom_open_file()
{
	int i,j;
	for(i=0;i<ROM_FILE_NUM;i++)
	{
		rom_file_name[i]=os_malloc(10);
		rom_file_name[i][0]='R';		//file name ROM0xxxx
		rom_file_name[i][1]='O';
		rom_file_name[i][2]='M';
		for(j=3;j<8;j++)
			rom_file_name[i][j]='0';
		rom_file_name[i][8]='\0';
	}

	for(i=0;i<ROM_FILE_NUM;i++)
		int_to_str(rom_file_name[i],i);
	for(i=0;i<ROM_FILE_NUM;i++)
	{
		rom_fp[i]=os_fopen(rom_file_name[i],"r+");
		if(rom_fp==NULL)
			os_panic("no rom file %s is found!",rom_file_name[i]);
	}
}

static void rom_close_file(void);
static void rom_close_file()
{
	int i;
	for(i=0;i<ROM_FILE_NUM;i++)
	{
		os_free(rom_file_name[i]);
		os_fclose(rom_fp[i]);
	}
}

static void *addr_to_fp(addr_t addr)
{
	int i;
	i= (addr-ROM_BASE)>>BUS_BAR_ADD_LINE;
	return(rom_fp[i]);
}


//define default memory operation. if there is no ROM/ROM, return 0xFF
static u32  rom_get_put(addr_t addr, u32 data, int size, int put);
static u32  rom_get_put(addr_t addr, u32 data, int size, int put)
{
    void *fp;
    int offset;

    offset=((1<<BUS_BAR_ADD_LINE)-1)&addr;
    fp=addr_to_fp(addr);

    os_fseek_set(fp,offset);

    switch (size) {
        case 4:
            if (put)
            {
            	os_panic("can't write 32bits at rom: %x",addr);
            }
            else
            	os_fread(fp, &data, 4);
            break;
        case 2:
            if (put)
            {
            	os_panic("can't write 16bits at rom: %x",addr);
            }
             else
             	os_fread(fp, &data, 2);
             break;
        case 1:
            if (put)
            {
            	os_panic("can't write 8bits at rom: %x",addr);
            }
             else
             	os_fread(fp, &data, 1);
             break;
    }
    return data;
}

static void *rom_get_ptr(addr_t addr);
static void *rom_get_ptr(addr_t addr)
{
    return NULL;
}

/*================================================================*/
void sys_ini_rom();
void sys_ini_rom()
{
	rom_open_file();
	bus_set_func(ROM_BASE,rom_get_put,rom_get_ptr);
	bus_bat_set_func(ROM_BASE,ROM_SIZE);
}

void sys_del_rom();
void sys_del_rom()
{
	rom_close_file();
}

#else

struct rom_t{
    /* main memory backing store */
    armaddr_t base;
    armaddr_t size;
    byte *host_mem;
} rom;

static dword rom_get_put(armaddr_t address, dword data, int size, int put);
static dword rom_get_put(armaddr_t address, dword data, int size, int put)
{
    void *ptr;

    ptr = rom.host_mem + (address - rom.base);

    switch (size) {
        case 4:
            if (!put)
                data = READ_MEM_DWORD(ptr);
            break;
        case 2:
            if (!put)
                data = READ_MEM_WORD(ptr);
            break;
        case 1:
            if (!put)
                data = READ_MEM_BYTE(ptr);
            break;
    }

    SYS_TRACE(5, "sys: rom_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    return data;
}


static void *rom_get_ptr(armaddr_t address);
static void *rom_get_ptr(armaddr_t address)
{
	DWORD offset;
	offset = address - rom.base;
	if(offset < rom.size)
		return rom.host_mem + offset;
	else
		return NULL;
}

/*================================================================*/
void sys_ini_rom(void);
void sys_ini_rom()
{
	int host_mem_len;
	void *host_mem_addr;


	host_mem_len = RAM_SIZE;
	host_mem_addr = os_malloc(host_mem_len);

	rom.host_mem = host_mem_addr;
	rom.base = RAM_BASE;
	rom.size = RAM_SIZE;
	bus_set_func(ROM_BASE,rom_get_put,rom_get_ptr);
	bus_set_host_base_addr(ROM_BASE, host_mem_addr);
	bus_bat_set_func(ROM_BASE,host_mem_len);
}

void sys_del_rom();
void sys_del_rom()
{
	void *host_mem_addr;
	host_mem_addr = rom.host_mem;
	os_free(host_mem_addr);
}

#endif
/*================================================================*/
/* end of sys_rom.c */
