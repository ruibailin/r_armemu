
/*------------------------------------
 * os_sdl2.h
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

#include "0ctr.h"
#ifdef	_OS_HAVE_C64_
#ifndef OS_HAVE_C64_OS_SDL2_H_
#define OS_HAVE_C64_OS_SDL2_H_
/*================================================================*/
#include "os_type.h"
/*================================================================*/
extern int os_sdl2_init_sdl(void);
extern void os_sdl2_quit_sdl(void);
extern int os_sdl2_init_sub_sys(void);
extern void os_sdl2_quit_sub_sys(void);
/*================================================================*/
extern void os_sdl2_create_window(const char *t,dword w, dword h);
extern void os_sdl2_distroy_window(void);
extern void os_sdl2_get_surface(void);
extern void os_sdl2_update_surface(void);
extern void os_sdl2_flush_window(void);
/*================================================================*/
extern void *os_sdl2_create_surface_from_bmp(const char *bmp);
extern void os_sdl2_destroy_surface(void *sf);
extern void os_sdl2_lock_surface(void *sf);
extern void os_sdl2_unlock_surface(void *sf);
extern void os_sdl2_flip_surface(void *sf);
/*================================================================*/
extern void os_sdl2_create_render(void);
extern void os_sdl2_destroy_render(void);
extern void os_sdl2_set_rgb_color(BYTE r,BYTE g,BYTE b);
extern void os_sdl2_draw_point(int x,int y);
extern void os_sdl2_draw_line(int x1, int y1, int x2, int y2);
extern void os_sdl2_draw_rect(int x,int y,int w,int h);

/*================================================================*/
extern int os_sdl2_poll_event(void);
extern bool os_sdl2_is_key_up(void);
extern bool os_sdl2_is_key_down(void);
extern dword os_sdl2_get_key_value(void);
extern const char *os_sdl2_get_key_name(void);
extern bool os_sdl2_is_key_esc(void);
extern bool os_sdl2_is_quit(void);
extern void os_sdl2_send_quit(void);

/*================================================================*/
extern void *os_sdl2_create_thread(int (*proc)(void *arg));
/*================================================================*/
extern int os_sdl2_add_timer(dword ms,dword (*timer)(dword intv, void *para));
extern void os_sdl2_del_timer(int timerid);
extern void os_sdl2_delay(dword ms);

/*================================================================*/
extern void *os_sdl2_create_mutex(void);
extern void os_sdl2_destroy_mutex(void *m);
extern void os_sdl2_lock_mutex(void *sc);
extern void os_sdl2_unlock_mutex(void *sc);

/*================================================================*/
#endif
#endif 
/* end of os_sdl2.h */
