
#include "reach_socket.h"
#include "media_msg.h"
#include "reach_os.h"

int32_t set_nonblocking(int32_t sock)
{
	int32_t opts;
	opts=fcntl(sock,F_GETFL);
	if(opts<0)
	{
		perror("fcntl(sock,GETFL)");
		return -1;
	}
	opts = opts|O_NONBLOCK;
	if(fcntl(sock,F_SETFL,opts)<0)
	{
		perror("fcntl(sock,SETFL,opts)");
		return -1;
	}   
	return 0;
}

void set_net_addr(sockaddr_in_t *addr, int8_t *ip, int32_t port)
{
	r_bzero(addr, sizeof(sockaddr_in_t));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip);
	addr->sin_port=htons(port);
}


void set_local_addr(sockaddr_un_t *local_addr, int8_t* unixstr_path)
{
	r_bzero(local_addr, sizeof(sockaddr_un_t));
	local_addr->sun_family = AF_LOCAL;
	r_strcpy(local_addr->sun_path, unixstr_path);
}


int32_t async_recv_data(int32_t fd, void *buf, int32_t buflen)
{
	
	int32_t ret = -1;
	char *recv_buf = buf;
	int32_t total = buflen;
	int32_t recv_len = 0;
	while(1) {
		recv_len = recv(fd, recv_buf, total, 0);
		if(recv_len < 0) {
			//perror("[async_recv_data]");
			if(errno == EAGAIN || errno == EINTR) {
				ret = -EAGAIN;
				break;
			} 
			else
			{
				ret = -1;  //other error
				break;
			}
		}
		if(0 == recv_len) {
			ret = -1;
			break;
		}
		if(total == recv_len) {
			ret = 0;
			break;
		}
		total -= recv_len;
		recv_buf += recv_len;
	}	
	return ret;
}


int32_t async_send_data(int32_t fd, void *buf, int32_t *buflen)
{

	int32_t i_loop = 0 ;
	int8_t *pbuf = buf;
	int32_t send_total_len = *buflen;
	int32_t real_len = 0;
	int32_t ret = -1;
	while(1) {
		real_len = send(fd, pbuf, send_total_len, 0);
		if(100 < i_loop++ )
		{	
			printf("real_len is --- [%d]\n",real_len);
			printf("async_send_data is send!\n");
			i_loop =0;
		}
		if(real_len <  0) {
			if(errno == EAGAIN|| errno == EINTR) {
				ret = -EAGAIN;
				*buflen = send_total_len;
				break;
			} 
			else
			{
				ret = -1;
				break;  //other error
			}
		}
		if(0 == real_len) {
			ret = -1;
			break;
		}
		/*���ݷ��������Ҫ������һ���ڵ�*/
		if((size_t)real_len == send_total_len) {
			ret = 0;
			break;
		}		
		send_total_len -= real_len;
		pbuf +=real_len;
	}
	return ret;
}

void close_socket(int32_t fd)
{
	shutdown(fd, SHUT_RDWR);
	close(fd);
}

