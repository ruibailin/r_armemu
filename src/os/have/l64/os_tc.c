
/*------------------------------------
 * os_tc.c
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
/*================================================================*/
#include <fcntl.h>
#include <termios.h>

/*================================================================*/
static struct termios oldstdin;
static struct termios oldstdout;

/*================================================================*/
void os_tc_reset_in(void);
void os_tc_reset_in(void)
{
    tcsetattr(0, TCSANOW, &oldstdin);
}

void os_tc_reset_out(void);
void os_tc_reset_out(void)
{
    tcsetattr(1, TCSANOW, &oldstdout);
}

void os_tc_set_in(void);
void os_tc_set_in(void)
{
    struct termios t;
    return;		//rbl
    tcgetattr(0, &oldstdin);
    tcgetattr(1, &oldstdout);

    t = oldstdin;
    t.c_lflag = ISIG; // no input processing
    // Don't interpret various control characters, pass them through instead
    t.c_cc[VINTR] = t.c_cc[VQUIT] = t.c_cc[VSUSP] = '\0';
    t.c_cc[VMIN]  = 0; // nonblocking read
    t.c_cc[VTIME] = 0; // nonblocking read
    tcsetattr(0, TCSANOW, &t);

    fcntl(0, F_SETFL, O_NONBLOCK);

}

void os_tc_set_out(void);
void os_tc_set_out(void)
{
    struct termios t;

    tcgetattr(1, &oldstdout);

    t = oldstdout;
    t.c_lflag = ISIG; // no output processing
    // Don't interpret various control characters, pass them through instead
    t.c_cc[VINTR] = t.c_cc[VQUIT] = t.c_cc[VSUSP] = '\0';
    tcsetattr(1, TCSANOW, &t);
}

/*================================================================*/
#endif
/* end of os_tc.c */
