
/*------------------------------------
 * test_sdl2.c
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
#if _UNIT_TEST_SDL2_
/*================================================================*/
#include "../1imp.h"
#include "rgb2yuv.h"
#include "yuv2rgb.h"

static void sdl_set_yuv_color(BYTE y,BYTE u,BYTE v);
static void sdl_set_yuv_color(BYTE y,BYTE u,BYTE v)
{
	BYTE r,g, b;
	r=yuv_get_r(y,u,v);
	g=yuv_get_g(y,u,v);
	b=yuv_get_b(y,u,v);
	os_sdl2_set_rgb_color(r,g,b);
}

static void app_draw_string(BYTE *out,int w,int h);
static void app_draw_string(BYTE *out,int w,int h)
{
	int i,j,k;
	k=0;
	for(i=0;i<h;i++)
		for(j=0;j<w;j++)
		{
	    	sdl_set_yuv_color(out[k],0,0);
	    	os_sdl2_draw_point(j,i);
			k++;
		}
}

static void blk_draw_string(BYTE *out,int x,int y,int w,int h);
static void blk_draw_string(BYTE *out,int x,int y,int w,int h)
{
	int i,j,k;
	k=0;
	for(i=0;i<h;i++)
		for(j=0;j<w;j++)
		{
	    	sdl_set_yuv_color(out[k],0,0);
	    	os_sdl2_draw_point(j+x,i+y);
			k++;
		}
}

/*================================================================*/
static void app_show_string(BYTE *out,int w,int h);
static void app_show_string(BYTE *out,int w,int h)
{
	bool done;
	done=FALSE;
	while (!done) {
	os_sdl2_poll_event();
	done=os_sdl2_iS_key_down();
	if(!done)
		continue;
//    done=os_sdl2_is_key_esc();
	if(done)
		break;
    app_draw_string(out,w,h);
    os_sdl2_flush_window();
    }
	done=0;
	while (!done) {
	os_sdl2_poll_event();
	done=os_sdl2_iS_key_down();
	if(!done)
		continue;
//	done=os_sdl2_is_key_esc();
	if(done)
		break;
    app_draw_string(out,w,h);
    os_sdl2_flush_window();
    }
}

void test_os_sdl2(void);
void test_os_sdl2()
{

	os_sdl2_init_sdl();
	os_sdl2_create_window("Test SDL2", 640, 480);
	os_sdl2_get_surface();
	os_sdl2_create_render();

	os_sdl2_set_rgb_color(0xff,0x00,0xff);
	os_sdl2_draw_rect(40,40,40,40);

	os_sdl2_destroy_render();
	os_sdl2_distroy_window();
	os_sdl2_quit_sdl();

}

/*================================================================*/
#endif
/* end of test_sdl2.c */
