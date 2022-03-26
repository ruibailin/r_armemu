
/*------------------------------------
 * os_sdl2.c
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
#if _OS_HAVE_SDL2_
/*================================================================*/
#include <SDL2/SDL.h>
#include "os_type.h"
#include "os_print.h"

/*================================================================*/
int os_sdl2_init_sdl(void);
int os_sdl2_init_sdl(void)
{
	int ret;
	ret = SDL_Init( SDL_INIT_VIDEO);
	if(ret<0)
	{
    	os_printf("Unable to initialize SDL: %d, %s\n", ret, SDL_GetError());
	}
    return ret;
}

void os_sdl2_quit_sdl(void);
void os_sdl2_quit_sdl(void)
{
	SDL_Quit();
}

int os_sdl2_init_sub_sys(void);
int os_sdl2_init_sub_sys(void)
{
	int ret;
	ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
	if(ret<0)
	{
    	os_printf("Unable to initialize SDL SubSystem: %d, %s\n", ret, SDL_GetError());
	}
    return ret;
}

void os_sdl2_quit_sub_sys(void);
void os_sdl2_quit_sub_sys(void)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

/*================================================================*/
static SDL_Window* sdl2_window;
static SDL_Surface* sdl2_surface;

void os_sdl2_create_window(const char *t,dword w, dword h);
void os_sdl2_create_window(const char *t,dword w, dword h)
{
	sdl2_window = SDL_CreateWindow( \
            t,  \
            SDL_WINDOWPOS_CENTERED, \
            SDL_WINDOWPOS_CENTERED, \
            w, \
            h, \
            SDL_WINDOW_SHOWN);

	if(sdl2_window==NULL)
	{
    	os_printf("Unable to create SDL Window: %s\n", SDL_GetError());
        SDL_Quit();
	}
}

void os_sdl2_distroy_window(void);
void os_sdl2_distroy_window(void)
{

	SDL_DestroyWindow( sdl2_window );
}


void os_sdl2_get_surface(void);
void os_sdl2_get_surface(void)
{
    sdl2_surface = SDL_GetWindowSurface( sdl2_window );
}

void os_sdl2_update_surface(void);
void os_sdl2_update_surface(void)
{
    SDL_UpdateWindowSurface( sdl2_window );
}

void os_sdl2_flush_window(void);
void os_sdl2_flush_window(void)
{
    SDL_UpdateWindowSurface( sdl2_window );
}
/*================================================================*/

void *os_sdl2_create_surface_from_bmp(const char *bmp);
void *os_sdl2_create_surface_from_bmp(const char *bmp)
{
	SDL_Surface* surface;

	surface = SDL_LoadBMP( bmp );
    return (void *)surface;
}

void os_sdl2_destroy_surface(void *sf);
void os_sdl2_destroy_surface(void *sf)
{
	SDL_Surface *surface;
	surface=(SDL_Surface *)sf;

	SDL_FreeSurface(surface);
}


void os_sdl2_lock_surface(void *sf);
void os_sdl2_lock_surface(void *sf)
{
	SDL_Surface *surface;
	surface=(SDL_Surface *)sf;

	SDL_LockSurface(surface);
}

void os_sdl2_unlock_surface(void *sf);
void os_sdl2_unlock_surface(void *sf)
{
	SDL_Surface *surface;
	surface=(SDL_Surface *)sf;

	SDL_UnlockSurface(surface);
}

void os_sdl2_flip_surface(void *sf);
void os_sdl2_flip_surface(void *sf)
{
	SDL_Surface *surface;
	surface=(SDL_Surface *)sf;

	SDL_BlitSurface( surface, NULL, sdl2_surface, NULL );

}

/*================================================================*/
static SDL_Renderer *sdl2_render;
void os_sdl2_create_render(void);
void os_sdl2_create_render()
{
	sdl2_render = SDL_CreateSoftwareRenderer(sdl2_surface);

	if(sdl2_render==NULL)
	{
    	os_printf("Unable to create SDL Render: %s\n", SDL_GetError());
        SDL_Quit();
	}
}

void os_sdl2_destroy_render(void);
void os_sdl2_destroy_render()
{
	SDL_DestroyRenderer(sdl2_render);
}

void os_sdl2_set_rgb_color(BYTE r,BYTE g,BYTE b);
void os_sdl2_set_rgb_color(BYTE r,BYTE g,BYTE b)
{
	SDL_SetRenderDrawColor(sdl2_render, r, g, b, 0xFF);
}

void os_sdl2_draw_point(int x,int y);
void os_sdl2_draw_point(int x,int y)
{
	SDL_RenderDrawPoint(sdl2_render, x,y);
}

void os_sdl2_draw_line(int x1, int y1, int x2, int y2);
void os_sdl2_draw_line(int x1, int y1, int x2, int y2)
{
	SDL_RenderDrawLine(sdl2_render, x1, y1, x2, y2);
}

void os_sdl2_draw_rect(int x,int y,int w,int h);
void os_sdl2_draw_rect(int x,int y,int w,int h)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderFillRect(sdl2_render, &rect);
}

/*================================================================*/
#include <SDL2/SDL_events.h>
static SDL_Event sdl2_event;

int os_sdl2_poll_event(void);
int os_sdl2_poll_event(void)
{
	int res;
	res=SDL_PollEvent(&sdl2_event);
	return res;
}

bool os_sdl2_is_key_up(void);
bool os_sdl2_is_key_up(void)
{
	if(sdl2_event.type == SDL_KEYUP)
		return TRUE;
	else
		return FALSE;
}

bool os_sdl2_is_key_down(void);
bool os_sdl2_iS_key_down(void)
{
	if(sdl2_event.type == SDL_KEYDOWN)
		return TRUE;
	else
		return FALSE;
}

dword os_sdl2_get_key_value(void);
dword os_sdl2_get_key_value(void)
{
	SDL_Keycode key;
	SDL_GetKeyName(sdl2_event.key.keysym.sym);
	key=sdl2_event.key.keysym.sym;
	return (dword)key;
}

const char *os_sdl2_get_key_name(void);
const char *os_sdl2_get_key_name(void)
{
	const char *name;
	name=SDL_GetKeyName(sdl2_event.key.keysym.sym);
	return name;
}

bool os_sdl2_is_key_esc(void);
bool os_sdl2_iS_key_esc(void)
{
	SDL_GetKeyName(sdl2_event.key.keysym.sym);
	if(sdl2_event.key.keysym.sym == SDLK_ESCAPE)
		return TRUE;
	else
		return FALSE;
}


bool os_sdl2_is_quit(void);
bool os_sdl2_iS_quit(void)
{
	if(sdl2_event.type == SDL_QUIT)
		return TRUE;
	else
		return FALSE;
}

void os_sdl2_send_quit(void);
void os_sdl2_send_quit(void)
{
    SDL_Event sdl2_event;

    sdl2_event.type = SDL_QUIT;
    SDL_PushEvent(&sdl2_event);
}

/*================================================================*/
#include <SDL2/SDL_thread.h>
void *os_sdl2_create_thread(int (*proc)(void *arg));
void *os_sdl2_create_thread(int (*proc)(void *arg))
{
	SDL_Thread *thd;
	thd = SDL_CreateThread(proc, NULL, NULL);
	return (void *)thd;
}

/*================================================================*/
int os_sdl2_add_timer(dword ms,dword (*timer)(dword intv, void *para));
int os_sdl2_add_timer(dword ms,dword (*timer)(dword intv, void *para))
{
	SDL_TimerCallback callback;
	SDL_TimerID tid;
	callback=timer;
	tid=SDL_AddTimer(ms, callback, NULL);
	return (int)tid;
}

void os_sdl2_del_timer(int timerid);
void os_sdl2_del_timer(int timerid)
{
	SDL_TimerID tid;
	tid = (SDL_TimerID )timerid;
	SDL_RemoveTimer(tid);
}

void os_sdl2_delay(dword ms);
void os_sdl2_delay(dword ms)
{
	 SDL_Delay(ms);
}

/*================================================================*/
#include <SDL2/SDL_mutex.h>
void *os_sdl2_create_mutex(void);
void *os_sdl2_create_mutex(void)
{
    SDL_mutex *mutex;
	mutex = SDL_CreateMutex();

	return (void *)(mutex);
}

void os_sdl2_destroy_mutex(void *m);
void os_sdl2_destroy_mutex(void *m)
{
    SDL_mutex *mutex;

	mutex = (SDL_mutex *)m;
	SDL_DestroyMutex(mutex);
}


void os_sdl2_lock_mutex(void *sc);
void os_sdl2_lock_mutex(void *sc)
{
    SDL_mutex *mutex;
	mutex = (SDL_mutex *)sc;

    SDL_LockMutex(mutex);

}

void os_sdl2_unlock_mutex(void *sc);
void os_sdl2_unlock_mutex(void *sc)
{
    SDL_mutex *mutex;
	mutex = (SDL_mutex *)sc;

    SDL_UnlockMutex(mutex);
}
/*================================================================*/
#endif
#endif
/* end of os_sdl2.c */
