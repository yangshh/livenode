
#ifndef __REACH_EPOLL_H__
#define __REACH_EPOLL_H__
#include <sys/epoll.h>
#include "stdint.h"
#define	MAXLINE		512
#define	INFTIM		1000
#define	MAX_CHANNEL_ID	500

typedef struct epoll_event ev_t;
int32_t add_epoll_event(int32_t epfd, int32_t evfd, ev_t *ev, int32_t events);
int32_t del_epoll_event(int32_t epfd, int32_t evfd, ev_t *ev, int32_t events);
int32_t add_net_listen_epoll_event(int32_t epfd, ev_t *ev, int32_t listenfd, int8_t *ip, uint16_t port);
int32_t add_local_listen_epoll_event(int32_t epfd, ev_t *ev, int32_t listenfd, char *unixstr_path);
#endif //__REACH_EPOLL_H__

