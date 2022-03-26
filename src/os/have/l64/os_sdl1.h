
/*------------------------------------
 * os_sdl1.h
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
#ifdef	_OS_HAVE_L64_
#if _OS_HAVE_SDL1_
#ifndef SRC_OS_HAVE_L64_OS_SDL1_H_
#define SRC_OS_HAVE_L64_OS_SDL1_H_
/*================================================================*/
#include "os_type.h"

extern int os_sdl1_init_sdl(void);
extern void os_sdl1_quit_sdl(void);
extern int os_sdl1_init_sub_sys(void);
extern void os_sdl1_quit_sub_sys(void);
/*================================================================*/
extern void os_sdl1_set_v_mod(dword x, dword y, dword depth);
extern void os_sdl1_update_rect(dword x, dword y, dword w,dword h);
extern void os_sdl1_lock_surface(void);
extern void *os_sdl1_get_surface_pixels(void);
extern void os_sdl1_unlock_surface(void);
extern void os_sdl1_flip_surface(void);
extern void os_sdl1_set_caption(const char *t,const char *icon);
/*================================================================*/
extern void os_sdl1_wait_event(void);
extern bool os_sdl1_is_key_up(void);
extern bool os_sdl1_is_key_down(void);
extern dword os_sdl2_get_key_value(void);
extern const char *os_sdl1_get_key_name(void);
extern bool os_sdl1_is_key_esc(void);
extern bool os_sdl1_is_quit(void);
extern void os_sdl1_send_quit(void);

/*================================================================*/
extern void *os_sdl1_create_thread(int (*proc)(void *arg));
extern void os_sdl1_destroy_thread(void *thd);
/*================================================================*/
extern void *os_sdl1_add_timer(dword ms,dword (*timer)(dword intv, void *para));
extern void os_sdl1_del_timer(void *timerid);
extern void os_sdl1_delay(dword ms);

/*================================================================*/
extern void *os_sdl1_create_mutex(void);
extern void os_sdl1_lock_mutex(void *sc);
extern void os_sdl1_unlock_mutex(void *sc);

/*================================================================*/
#endif
#endif
#endif 
/* end of os_sdl1.h */
