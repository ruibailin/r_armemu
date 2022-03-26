
/*------------------------------------
 * os_skt_clt.c
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
/*================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "os_print.h"
/*================================================================*/
static int os_skt_fd;
static void clt_create_skt(void);
static void clt_create_skt(void)
{
    os_skt_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(os_skt_fd>=0)
    	return;
	os_print("Client create socket error: %s(errno: %d)\n", strerror(errno),errno);
}

static struct sockaddr_in svr_skt_addr;
static const char *svr_ip_addr="127.0.0.1";
static void clt_bind_skt(void);
static void clt_bind_skt(void)
{
    if( os_skt_fd < 0)
    	return;

    memset(&svr_skt_addr, 0, sizeof(svr_skt_addr));
    svr_skt_addr.sin_family = AF_INET;
    svr_skt_addr.sin_port = htons(APP_SKT_PORT);

    int ret;
    ret=inet_pton(AF_INET, svr_ip_addr, &svr_skt_addr.sin_addr);
    if( ret >0 )
    	return;

    os_print("inet_pton error for %s\n",svr_ip_addr);
    close(os_skt_fd);
    os_skt_fd=-1;
}

static void clt_connect_skt(void);
static void clt_connect_skt(void)
{
	int ret;
    ret = connect(os_skt_fd, (struct sockaddr*)&svr_skt_addr, sizeof(svr_skt_addr));
    if( ret >= 0)
    	return;

   	os_print("Client connect error: %s(errno: %d)\n",strerror(errno),errno);
   	close(os_skt_fd);
   	os_skt_fd=-1;
}

static void clt_disconnect_skt(void);
static void clt_disconnect_skt(void)
{

}

static void clt_destroy_skt(void);
static void clt_destroy_skt(void)
{
   	os_print("Client disconnect socket %d\n",os_skt_fd);
   	close(os_skt_fd);
   	os_skt_fd=-1;
}

/*================================================================*/
void os_clt_init_skt(void);
void os_clt_init_skt(void)
{
    clt_create_skt();
    clt_bind_skt();

	clt_connect_skt();
}

void os_clt_free_skt(void);
void os_clt_free_skt(void)
{
	clt_disconnect_skt();

	clt_destroy_skt();
}


static const char *magic="Received!";
void os_clt_send_skt(char *buf,int size);
void os_clt_send_skt(char *buf,int size)
{
	if(os_skt_fd < 0)
		return;

	int sen_len;

    sen_len = send(os_skt_fd, buf, size, 0);
    if( sen_len != size)
    	goto send_err;

    int rec_len;
    char rec_buf[16];
    rec_len = recv(os_skt_fd, rec_buf, strlen(magic),0);
    if(rec_len > 0)
    	return;
send_err:
	os_print("Client send buffer error,%d\n",sen_len);
	close(os_skt_fd);
	os_skt_fd=-1;
}

void os_clt_recv_skt(char *buf,int size);
void os_clt_recv_skt(char *buf,int size)
{
    int rec_len;
	int sen_len;

	if(os_skt_fd < 0)
		return;
    rec_len = recv(os_skt_fd, buf, size,0);
    if(rec_len != size)
    	goto recv_err;
    sen_len = send(os_skt_fd, magic, strlen(magic), 0);
    if( sen_len != -1)
    	return;

recv_err:
	os_print("Client receive buffer error\n");
	close(os_skt_fd);
	os_skt_fd=-1;
}

/*================================================================*/
#endif
/* end of os_skt_clt.c */
