
#include "reach_epoll.h"
#include "reach_socket.h"
#include "reach_list.h"
#include "key.h"
#define	MAXLINE		512
#define	OPEN_MAX	100
#define	USER_PORT	4520
#define	SILVERLIGHT_PORT	943
#define	SERV_IP		"0.0.0.0"
#define	INFTIM		1000
#define	MAX_CHANNEL_ID	500

int32_t add_epoll_event(int32_t epfd, int32_t evfd, ev_t *ev, int32_t events)
{
	//设置与要处理的事件相关的文件描述符
	ev->data.fd=evfd;
	//设置要处理的事件类型
	ev->events = events;
	//注册epoll事件
	if(epoll_ctl(epfd,EPOLL_CTL_ADD, evfd, ev) < 0)
	{
		printf("add_epoll_event ----- \n");
		perror("[add_epoll_event] epoll_ctl ");
		printf("[add_epoll_event] evfd : [%d]\n", evfd);
		return -1;
	}
	return 0;
}

int32_t del_epoll_event(int32_t epfd, int32_t evfd, ev_t *ev, int32_t events)
{
	//设置与要处理的事件相关的文件描述符
	ev->data.fd=evfd;
	//设置要处理的事件类型
	ev->events = events;
	//注册epoll事件
	if(epoll_ctl(epfd,EPOLL_CTL_DEL, evfd, ev) < 0)
	{
		perror("[del_epoll_event] epoll_ctl ");
		printf("[del_epoll_event] evfd : [%d]\n", evfd);
		return -1;
	}
	return 0;
}


int32_t add_net_listen_epoll_event(int32_t epfd, ev_t *ev, int32_t listenfd, int8_t *ip, uint16_t port)
{
	sockaddr_in_t serveraddr;
	set_nonblocking(listenfd);
	add_epoll_event(epfd, listenfd, ev, EPOLLIN|EPOLLET);
	set_net_addr(&serveraddr, ip, port);
	int on = 1;
	setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	if(bind(listenfd,(sa_t *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("[add_net_listen_epoll_event] bind : ");
		return -1;
	}
	if(listen(listenfd, LISTENQ) < 0)
	{
		perror("[add_net_listen_epoll_event] listen :");
		return -1;
	}
	return 0;
}

int32_t add_local_listen_epoll_event(int32_t epfd, ev_t *ev, int32_t listenfd, char *unixstr_path)
{
	sockaddr_un_t local_addr;
	set_nonblocking(listenfd);
	add_epoll_event(epfd, listenfd, ev, EPOLLIN|EPOLLET);
	set_local_addr(&local_addr, unixstr_path);
	int on = 1;
	setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	if(bind(listenfd,(struct sockaddr *)&local_addr, sizeof(local_addr))<0)
	{
		perror("[add_local_listen_epoll_event] bind : ");
		return -1;
	}
	if(listen(listenfd, LISTENQ))
	{
		perror("[add_local_listen_epoll_event] listen :");
		return -1;
	}
	return 0;
}




