
/*------------------------------------
 * os_skt_svr.c
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
static  int    os_skt_fd, os_cnct_fd;
/*================================================================*/
static void svr_create_skt(void);
static void svr_create_skt(void)
{
    os_skt_fd = socket(AF_INET, SOCK_STREAM, 0);
    if( os_skt_fd != -1 )
    	return;
   	os_print("create socket error: %s(errno: %d)\n",strerror(errno),errno);
}

static void svr_bind_skt(void);
static void svr_bind_skt(void)
{
	if(os_skt_fd == -1)
		return;

	struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(APP_SKT_PORT);

    int ret;
    ret=bind(os_skt_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if( ret != -1)
    	return;

    os_print("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
    close(os_skt_fd);
    os_skt_fd=-1;
   	return;
}

static void svr_listen_skt(void);
static void svr_listen_skt(void)
{
	if(os_skt_fd == -1)
		return;

    int ret;
    ret=listen(os_skt_fd, 10);
    if( ret != -1)
    	return;

    os_print("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
    close(os_skt_fd);
    os_skt_fd=-1;
}

static void svr_connect_skt(void);
static void svr_connect_skt(void)
{
	if(os_skt_fd == -1)
		return;

	os_print("======waiting for client's request======\n");
    os_cnct_fd = accept(os_skt_fd, (struct sockaddr*)NULL, NULL);
    if( os_cnct_fd != -1)
    	return;

   	os_print("Server accept socket error: %s(errno: %d)",strerror(errno),errno);
    close(os_skt_fd);
    os_skt_fd=-1;
}

static void svr_disconnect_skt(void);
static void svr_disconnect_skt(void)
{
	if(os_skt_fd == -1)
		return;

	os_print("Server connect %d successfully released!\n",os_cnct_fd);
	close(os_cnct_fd);
	os_cnct_fd=-1;
}

static void svr_destroy_skt(void);
static void svr_destroy_skt(void)
{
	if(os_skt_fd == -1)
		return;

    close(os_skt_fd);
    os_skt_fd=-1;
}
/**************************************************/
void os_svr_init_skt(void);
void os_svr_init_skt(void)
{
    svr_create_skt();
    svr_bind_skt();
    svr_listen_skt();

	svr_connect_skt();
}

void os_svr_free_skt(void);
void os_svr_free_skt(void)
{
	svr_disconnect_skt();

	svr_destroy_skt();
}


static const char *magic="Received!";
void os_svr_recv_skt(char *buf,int size);
void os_svr_recv_skt(char *buf,int size)
{
	if(os_skt_fd == -1)
		return;

	if(os_cnct_fd == -1)
		return;

	int rec_len;
	rec_len = recv(os_cnct_fd, buf, size, 0);
    if(rec_len != size)
    	goto recv_err;

	int sen_len;
	sen_len=send(os_cnct_fd, magic, strlen(magic),0);
    if(sen_len != -1)
    	return;

recv_err:
	os_print("Server receive buffer error\n");
	close(os_cnct_fd);
	os_cnct_fd=-1;
}

void os_svr_send_skt(char *buf,int size);
void os_svr_send_skt(char *buf,int size)
{
	if(os_skt_fd == -1)
		return;

	if(os_cnct_fd == -1)
		return;

	int sen_len;
	sen_len=send(os_cnct_fd, buf, size, 0);
    if(sen_len != size)
    	goto send_err;

    int rec_len;
    char rec_buf[16];
    rec_len = recv(os_cnct_fd, rec_buf, strlen(magic), 0);
    if(rec_len > 0)
       	return;

send_err:
	os_print("Server receive buffer error\n");
	close(os_cnct_fd);
	os_cnct_fd=-1;
}

/*================================================================*/
#endif
/* end of os_skt_svr.c */
