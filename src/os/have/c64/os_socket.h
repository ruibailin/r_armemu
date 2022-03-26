
/*------------------------------------
 * os_socket.h
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
#ifdef	_OS_HAVE_C64_
#ifndef OS_HAVE_C64_OS_SOCKET_H_
#define OS_HAVE_C64_OS_SOCKET_H_
/*================================================================*/
//client call
extern void os_clt_init_skt(void);
extern void os_clt_free_skt(void);
extern void os_clt_send_skt(char *frm,int size);
extern void os_clt_recv_skt(char *frm,int size);
//sever call
extern void os_svr_init_skt(void);
extern void os_svr_free_skt(void);
extern void os_svr_recv_skt(char *frm,int size);
extern void os_svr_send_skt(char *frm,int size);
/*================================================================*/
#endif
#endif 
/* end of os_socket.h */
