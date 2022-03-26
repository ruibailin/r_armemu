
/*------------------------------------
 * sys_ram.c
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
#define	RAM_FILE_NUM  RAM_SIZE/MEM_SLOT_SIZE
static char *ram_file_name[RAM_FILE_NUM];
static void *ram_fp[RAM_FILE_NUM];

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

static void ram_open_file(void);
static void ram_open_file()
{
	int i,j;
	for(i=0;i<RAM_FILE_NUM;i++)
	{
		ram_file_name[i]=os_malloc(10);
		ram_file_name[i][0]='R';		//file name RAM0xxxx
		ram_file_name[i][1]='A';
		ram_file_name[i][2]='M';
		for(j=3;j<8;j++)
			ram_file_name[i][j]='0';
		ram_file_name[i][8]='\0';
	}

	for(i=0;i<RAM_FILE_NUM;i++)
		int_to_str(ram_file_name[i],i);
	for(i=0;i<RAM_FILE_NUM;i++)
	{
		ram_fp[i]=os_fopen(ram_file_name[i],"r+");
		if(ram_fp==NULL)
			os_panic("no ram file %s is found!",ram_file_name[i]);
	}
}

static void ram_close_file(void);
static void ram_close_file()
{
	int i;
	for(i=0;i<RAM_FILE_NUM;i++)
	{
		os_free(ram_file_name[i]);
		os_fclose(ram_fp[i]);
	}
}

static void *addr_to_fp(addr_t addr)
{
	int i;
	i= (addr-RAM_BASE)>>BUS_BAR_ADD_LINE;
	return(ram_fp[i]);
}



static u32  ram_get_put(addr_t addr, u32 data, int size, int put);
static u32  ram_get_put(addr_t addr, u32 data, int size, int put)
{
    void *fp;
    int offset;

    offset=((1<<BUS_BAR_ADD_LINE)-1)&addr;
    fp=addr_to_fp(addr);

    os_fseek_set(fp,offset);

    switch (size) {
        case 4:
            if (put)
            	os_fwrite(fp, &data, 4);
            else
            	os_fread(fp, &data, 4);
            break;
        case 2:
            if (put)
             	os_fwrite(fp, &data, 2);
             else
             	os_fread(fp, &data, 2);
             break;
        case 1:
            if (put)
             	os_fwrite(fp, &data, 1);
             else
             	os_fread(fp, &data, 1);
             break;
    }
    return data;
}


/*================================================================*/
void sys_ini_ram();
void sys_ini_ram()
{
	ram_open_file();
	bus_set_func(RAM_BASE,ram_get_put,NULL);
	bus_set_host_addr(RAM_BASE, 0);
	bus_bat_set_func(RAM_BASE,RAM_SIZE);
}

void sys_del_ram();
void sys_del_ram()
{
	ram_close_file();
}

#else

struct ram_t{
    /* main memory backing store */
    armaddr_t base;
    armaddr_t size;
    byte *host_mem;
} ram;

static dword ram_get_put(armaddr_t address, dword data, int size, int put);
static dword ram_get_put(armaddr_t address, dword data, int size, int put)
{
    void *ptr;

    ptr = ram.host_mem + (address - ram.base);

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

    SYS_TRACE(5, "sys: ram_get_put at 0x%08x, data 0x%08x, size %d, put %d\n",
              address, data, size, put);

    return data;
}


static void *ram_get_ptr(armaddr_t address);
static void *ram_get_ptr(armaddr_t address)
{
	DWORD offset;
	offset = address - ram.base;
	if(offset < ram.size)
		return ram.host_mem + offset;
	else
		return NULL;
}

/*================================================================*/
extern const char *elf_bin_name;
void sys_ini_ram(void);
void sys_ini_ram()
{
	int host_mem_len;
	void *host_mem_addr;
	void *fp;

	host_mem_len = RAM_SIZE;
	host_mem_addr = os_malloc(host_mem_len);

	ram.host_mem = host_mem_addr;
	ram.base = RAM_BASE;
	ram.size = RAM_SIZE;
	bus_set_func(RAM_BASE,ram_get_put,ram_get_ptr);
	bus_set_host_base_addr(RAM_BASE, host_mem_addr);
	bus_bat_set_func(RAM_BASE,host_mem_len);


	fp = os_fopen(elf_bin_name,"r");
	if(fp)
	{
		os_fread(fp,ram.host_mem,ram.size);
		os_fclose(fp);
	}
	else
		os_printf("Error,there is no elf file: %s,The reason is %s ! \n",elf_bin_name,os_ferror_name());
}

void sys_del_ram(void);
void sys_del_ram()
{
	void *host_mem_addr;
	host_mem_addr = ram.host_mem;
	os_free(host_mem_addr);
}




#endif
/*================================================================*/

/* end of sys_ram.c */
