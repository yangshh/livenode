
#include "events_process.h"
#include "reach_list.h"
#include "reach_epoll.h"
#include "reach_socket.h"
#include "user.h"
#include "media_msg.h"
#include "key.h"
#include "local_data.h"
#include "live_cfg.h"
static int32_t events_data_process(int32_t ev_datafd, int32_t net_listenfd, int32_t local_listenfd,  int32_t epfd, ev_t *ev)
{
	sockaddr_in_t clientaddr;
	sockaddr_un_t local_clientaddr;
	int32_t net_connfd = -1,  local_connfd = -1;
	user_t *user_handle = NULL;
	socklen_t clilen = 0;

	if(ev_datafd == net_listenfd) {
		printf("[events_data_process] net_listenfd : [%d]\n", net_listenfd);

		while(1) {
			clilen = sizeof(sockaddr_in_t);
			net_connfd = accept(net_listenfd, (sa_t *)&clientaddr, &clilen);

			if(net_connfd < 0) {
				printf("[events_data_process] errno : %d\n", errno);
				//perror("[events_process] net_listenfd  accept");
				break;
			}

			set_nonblocking(net_connfd);
			char *ip = inet_ntoa(clientaddr.sin_addr);
			printf("[events_data_process]client ip : [%s][%d]\n", ip, net_connfd);
			printf(" [events_data_process]EPOLLIN|EPOLLOUT|EPOLLET = %d \n", net_connfd);
			add_net_client_info_table(epfd, net_connfd, ev, EPOLLIN | EPOLLOUT | EPOLLET, ip);
		}
	} else if(ev_datafd == local_listenfd) {
		printf("[events_data_process] local_data_listenfd : [%d]\n", local_listenfd);

		while(1) {
			clilen = sizeof(sockaddr_un_t);
			local_connfd = accept(local_listenfd, (sa_t *)&local_clientaddr, &clilen);

			if(local_connfd < 0) {
				//perror("accept");
				break;
			}

			set_nonblocking(local_connfd);
			printf("[events_data_process]client path :[%d] [%s] [%d]\n", local_connfd, local_clientaddr.sun_path, clilen);
			user_handle = find_user_handle(local_connfd);

			if(NULL == user_handle) {
				add_local_client_info_table(epfd, local_connfd, ev, EPOLLIN | EPOLLET);
			}
		}
	}

	return 0;
}

static int32_t events_ev_process(uint32_t events, int32_t ev_datafd)
{
	user_t *user_handle = NULL;

	if((events & EPOLLERR) ||
	   (events & EPOLLHUP)) {
		user_handle = find_user_handle(ev_datafd);
		printf("[events_ev_process] epoll_wait error!!! fd : [%d] [%p]\n", ev_datafd, user_handle);

		if(NULL != user_handle) {
			close_client(user_handle);
		} else {
			close_socket(ev_datafd);
		}

		return -1;
	}

	if(events & EPOLLIN) {
		//printf("[epoll_wait]EPOLLIN !!!\n ");
		user_handle = find_user_handle(ev_datafd);

		if(NULL != user_handle) {
			if(user_handle->recv_process) {
				//printf("[events_process] recv_process XXXXXXXX\n");
				user_handle->recv_process(user_handle);
			}

			send_user_process();
			//printf("[events_process] send_user_process 000000000000000\n");
		}
	}

	if(events & EPOLLOUT) {
		//	printf("[events_process]EPOLLOUT !!!\n ");
		user_handle = find_user_handle(ev_datafd);

		if(NULL != user_handle)  {
			if(user_handle->send_process) {
				user_handle->send_process(user_handle);
			}
		}
	}

	return 0;

}

void* events_process(void *arg)
{
	int32_t i;
	int32_t net_listenfd = -1, local_data_listenfd = -1;
	int32_t epfd, nfds;
	int8_t local_ip[IP_LEN] = {0};
	int8_t *ifname = NULL;
	ifname = get_live_cfg_NetCard();
	if(NULL == ifname) {
		printf("get cfg netcard failed!!!\n");
	}
	if(get_local_ip(ifname, local_ip) < 0) {
		printf("[events_process] ----error----- [%s:%s]\n", ifname, local_ip);
	}
		
	//声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
	ev_t ev, events[EV_COUNT];
	//生成用于处理accept的epoll专用的文件描述符
	epfd=epoll_create(MAX_EPOLL_FD);

	unlink(LOACAL_DATA_MSG);
	local_data_listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	net_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("[events_process] local_data_listenfd : [%d], net_listenfd : [%d]\n", local_data_listenfd, net_listenfd);
	add_local_listen_epoll_event(epfd, &ev, local_data_listenfd, LOACAL_DATA_MSG);
	add_net_listen_epoll_event(epfd, &ev, net_listenfd, local_ip, USER_PORT);

	while(1) {
		//等待epoll事件的发生
		nfds = epoll_wait(epfd, events, EV_COUNT, EV_TIMEOUT);

		//printf("[events_process] epoll_wait after!!!nfds : [%d]\n", nfds);
		///处理所发生的所有事件
		for(i = 0; i < nfds; ++i) {
			if(0 > events_ev_process(events[i].events, events[i].data.fd)) {
				continue;
			}

			events_data_process(events[i].data.fd, net_listenfd, local_data_listenfd, epfd, &ev);
		}
	}

	//	printf("******\n");
	return NULL;
}



