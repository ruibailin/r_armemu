
/*------------------------------------
 * os_main.c
 * Create:  2021-01-30
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
#ifdef	_OS_HAVE_L64_
/*================================================================*/
extern int no_main(int argc, char **argv);

int os_main(int argc, char **argv);
int os_main(int argc, char **argv) {
	int ret;
	no_main(argc,argv);
	ret=0;
	return ret;
}
/*================================================================*/
#endif
/* end of os_main.c */
