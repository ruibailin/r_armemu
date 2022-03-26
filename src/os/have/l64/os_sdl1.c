
/*------------------------------------
 * os_sdl1.c
 * Create:  2021-02-17
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
/*================================================================*/
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include "os_type.h"
#include "os_print.h"

/*================================================================*/
int os_sdl1_init_sdl(void);
int os_sdl1_init_sdl(void)
{
	int ret;
	ret = SDL_Init(SDL_INIT_TIMER);
	if(ret<0)
	{
    	os_printf("Unable to initialize SDL: %d, %s\n", ret, SDL_GetError());
	}
    return ret;
}

void os_sdl1_quit_sdl(void);
void os_sdl1_quit_sdl(void)
{
	SDL_Quit();
}


int os_sdl1_init_sub_sys(void);
int os_sdl1_init_sub_sys(void)
{
	int ret;
	ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
	if(ret<0)
	{
    	os_printf("Unable to initialize SDL SubSystem: %d, %s\n", ret, SDL_GetError());
	}
    return ret;
}

void os_sdl1_quit_sub_sys(void);
void os_sdl1_quit_sub_sys(void)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


/*================================================================*/
static SDL_Surface* sdl1_surface;

void os_sdl1_set_v_mod(dword x, dword y, dword depth);
void os_sdl1_set_v_mod(dword x, dword y, dword depth)
{
	sdl1_surface = SDL_SetVideoMode(x, y, depth, SDL_HWSURFACE|SDL_DOUBLEBUF);

}

void os_sdl1_update_rect(dword x, dword y, dword w,dword h);
void os_sdl1_update_rect(dword x, dword y, dword w,dword h)
{
	SDL_UpdateRect(sdl1_surface, x,y,w,h);
}

void os_sdl1_lock_surface(void);
void os_sdl1_lock_surface()
{
	SDL_LockSurface(sdl1_surface);
}

void *os_sdl1_get_surface_pixels(void);
void *os_sdl1_get_surface_pixels()
{
	return sdl1_surface->pixels;
}


void os_sdl1_unlock_surface(void);
void os_sdl1_unlock_surface()
{
	SDL_UnlockSurface(sdl1_surface);
}

void os_sdl1_flip_surface(void);
void os_sdl1_flip_surface()
{
	SDL_Flip(sdl1_surface);
}

void os_sdl1_set_caption(const char *t,const char *icon);
void os_sdl1_set_caption(const char *t,const char *icon)
{
    SDL_WM_SetCaption(t,icon);
}

/*================================================================*/
static SDL_Event sdl1_event;

void os_sdl1_wait_event(void);
void os_sdl1_wait_event(void)
{
    SDL_WaitEvent(&sdl1_event);
}

bool os_sdl1_is_key_up(void);
bool os_sdl1_is_key_up(void)
{
	if(sdl1_event.type == SDL_KEYUP)
		return TRUE;
	else
		return FALSE;
}

bool os_sdl1_is_key_down(void);
bool os_sdl1_is_key_down(void)
{
	if(sdl1_event.type == SDL_KEYDOWN)
		return TRUE;
	else
		return FALSE;
}

dword os_sdl1_get_key_value(void);
dword os_sdl1_get_key_value(void)
{
	SDLKey key;
	SDL_GetKeyName(sdl1_event.key.keysym.sym);
	key=sdl1_event.key.keysym.sym;
	return (dword)key;
}

const char *os_sdl1_get_key_name(void);
const char *os_sdl1_get_key_name(void)
{
	const char *name;
	name=SDL_GetKeyName(sdl1_event.key.keysym.sym);
	return name;
}

bool os_sdl1_is_key_esc(void);
bool os_sdl1_is_key_esc(void)
{
	SDL_GetKeyName(sdl1_event.key.keysym.sym);
	if(sdl1_event.key.keysym.sym == SDLK_ESCAPE)
		return TRUE;
	else
		return FALSE;
}


bool os_sdl1_is_quit(void);
bool os_sdl1_is_quit(void)
{
	if(sdl1_event.type == SDL_QUIT)
		return TRUE;
	else
		return FALSE;
}

void os_sdl1_send_quit(void);
void os_sdl1_send_quit(void)
{
    SDL_Event event;

    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

/*================================================================*/
void *os_sdl1_create_thread(int (*proc)(void *arg));
void *os_sdl1_create_thread(int (*proc)(void *arg))
{
	SDL_Thread *thd;
	thd = SDL_CreateThread(proc, NULL);
	return (void *)thd;
}

void os_sdl1_destroy_thread(void *thd);
void os_sdl1_destroy_thread(void *thd)
{

	SDL_KillThread((SDL_Thread *)thd);
}
/*================================================================*/
void *os_sdl1_add_timer(dword ms,dword (*timer)(dword intv, void *para));
void *os_sdl1_add_timer(dword ms,dword (*timer)(dword intv, void *para))
{
	SDL_NewTimerCallback callback;
	SDL_TimerID tid;
	callback=timer;
	tid=SDL_AddTimer(ms, callback, NULL);
	return (void *)tid;
}

void os_sdl1_del_timer(void *timerid);
void os_sdl1_del_timer(void *timerid)
{
	SDL_TimerID tid;
	tid = (SDL_TimerID )timerid;
	SDL_RemoveTimer(tid);
}

void os_sdl1_delay(dword ms);
void os_sdl1_delay(dword ms)
{
	 SDL_Delay(ms);
}

/*================================================================*/
void *os_sdl1_create_mutex(void);
void *os_sdl1_create_mutex(void)
{
    SDL_mutex *mutex;
	mutex = SDL_CreateMutex();

	return (void *)(mutex);
}

void os_sdl1_lock_mutex(void *sc);
void os_sdl1_lock_mutex(void *sc)
{
    SDL_mutex *mutex;
	mutex = (SDL_mutex *)sc;

    SDL_LockMutex(mutex);

}

void os_sdl1_unlock_mutex(void *sc);
void os_sdl1_unlock_mutex(void *sc)
{
    SDL_mutex *mutex;
	mutex = (SDL_mutex *)sc;

    SDL_UnlockMutex(mutex);
}
/*================================================================*/
#endif
#endif
/* end of os_sdl1.c */
