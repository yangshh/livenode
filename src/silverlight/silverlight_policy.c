
#include "stdint.h"
#include "reach_os.h"
#include "reach_socket.h"
#include "key.h"
#include "events_process.h"
#include "silverlight_policy.h"
#define RLEN	128
int32_t read_silverlight_policy_file(int8_t **policy_data)
{
	FILE *fp = NULL;
	int32_t file_len = 0;
	int32_t len = 0;
	char *pbuf = NULL; 
	fp = fopen(SILVERLIGHT_POLICY_FILE, "rb");
	if(NULL == fp) 
	{ 
		return -1;
	}
	fseek(fp, 0L, SEEK_END); 
	file_len   =   ftell(fp);   //得到文件长度 
	fseek(fp, 0L, SEEK_SET);
	pbuf = (char *)malloc(file_len);
	memset(pbuf,0x00,file_len);
	len = fread(pbuf, sizeof(char), file_len, fp);
	*policy_data = pbuf;
	return len;
}

void* silverlight_policy_process(void *arg)
{
	printf("[silverlight_policy_process] enter!!\n");
	int32_t listenfd = -1, connfd = -1;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	sockaddr_in_t serveraddr, clientaddr;
	set_net_addr(&serveraddr, SERV_IP, SILVERLIGHT_PORT);
	int32_t on = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(bind(listenfd,(sa_t *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("[add_net_listen_epoll_event] bind : ");
		return NULL;
	}
	if(listen(listenfd, LISTENQ) < 0)
	{
		perror("[add_net_listen_epoll_event] listen :");
		return NULL;
	}

	socklen_t  len=sizeof(sockaddr_in_t);
	//int8_t *sbuf;
	int32_t sendlen;
	//sendlen = read_silverlight_policy_file(&sbuf);
	//if(-1 == sendlen) {
	///	return NULL;
	//}
	int8_t sbuf[SPX_LEN] = {0};
	sendlen = strlen(SILVERLIGHT_POLICY_XML);
	strcpy(sbuf, SILVERLIGHT_POLICY_XML);
	int8_t rbuf[RLEN] = {0};
	while(1)
	{
		len=sizeof(sockaddr_in_t);
		printf("[silverlight_policy_process] accept before !!!\n");
		//等待客户请求到来
		connfd = accept(listenfd, (sa_t *)&clientaddr, &len);
		if(connfd < 0) {
			printf("[events_process] errno : %d\n", errno);
			perror("[events_process] net_listenfd  accept");
			continue;
		}
		printf("[silverlight_policy_process] connfd : [%d] ip:[%s]\n", connfd, inet_ntoa(clientaddr.sin_addr));
		//接收数据
		r_memset(rbuf, 0, RLEN);
		recv(connfd, rbuf, RLEN, 0);
		printf("[silverlight_policy_process] recv : [%s]\n", rbuf);
		if (r_memcmp(SILVERLIGHT_POLICY_MSG, rbuf, 
					strlen(SILVERLIGHT_POLICY_MSG)) == 0) {
			//printf("[%s]\n", sbuf);
			send(connfd, sbuf, sendlen, 0);
		} else {
			printf("[silverlight_policy_process] fail : [%s]\n", rbuf);
		}
		//关闭套接字
		close(connfd);
	}
	//r_free(sbuf);
	//sbuf = NULL;
}


