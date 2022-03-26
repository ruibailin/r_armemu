
/*------------------------------------
 * test_sdl1.c
 * Create:  2021-02-18
 * Author:  Steve Rui
 *------------------------------------
 * Record:
 *
 *
 *
 *
 *------------------------------------
 */

#include "../0ctr.h"
#if	_UNIT_TEST_SDL1_
/*================================================================*/
#include "../1imp.h"

static void test_key(void);
static void test_key()
{
	bool done;
	const char *name;
	done=FALSE;
	while (!done) {
	os_sdl1_wait_event();
	done=os_sdl1_is_key_down();
	if(!done)
		continue;
	name = os_sdl1_get_key_name();
	os_printf("Input key is: %s",name);
    done=os_sdl1_is_key_esc();
	if(done)
		break;
	os_sdl1_update_rect(0,0,0,0);				//flash screen
    }
}


void test_os_sdl1(void);
void test_os_sdl1()
{

	os_sdl1_init_sdl();
	os_sdl1_init_sub_sys();
	os_sdl1_set_v_mod(640, 480, 32);
	os_sdl1_set_caption("Press ESC to quit!","c_soc");
	os_sdl1_update_rect(0,0,0,0);				//flash screen

	test_key();

	os_sdl1_quit_sub_sys();
	os_sdl1_quit_sdl();

}


/*================================================================*/
#endif
/* end of test_sdl1.c */
