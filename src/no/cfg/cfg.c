
/*------------------------------------
 * cfg.c
 * Create:  2021-02-20
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
#include "cfg_parse.h"

extern const char *cfg_file_name;
void cfg_ini_all(void);
void cfg_ini_all()
{
	int i;
//	i = cfg_parse_file("/home/steve/code/code/qemu/arm/c_soc/Default/armemu.conf");
	i = cfg_parse_file(cfg_file_name);
	os_printf("Total configuration line is: %d \n",i);
}

/*================================================================*/
/* end of cfg.c */
