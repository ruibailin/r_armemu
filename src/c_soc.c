
/*------------------------------------
 * soc.c
 * Create:  2021-01-24
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */


/*================================================================*/
extern int os_main(int argc, char **argv);
//extern void test_os_sdl1();

//when debug in eclipse, we need to use file name with absolute directory.
#if 1
const char *cfg_file_name="/home/steve/code/ruib/arm32/r_armemu/src/armemu.conf";
const char *elf_bin_name="/home/steve/code/ruib/arm32/r_armemu/src/test.bin";
const char *blk_dev_name="/home/steve/code/ruib/arm32/r_armemu/src/bdev.bin";
#else
const char *cfg_file_name="../src/armemu.conf";
const char *elf_bin_name="../src/test.bin";
const char *blk_dev_name="../src/bdev.bin";
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv) {
	int ret;

	ret=os_main(argc,argv);
	return ret;
}

/*================================================================*/

/* end of soc.c */
