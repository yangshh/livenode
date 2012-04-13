#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h> 
#include <netdb.h> 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <signal.h>
#include<semaphore.h>
#include <sys/wait.h>
#include "reach_os.h"
#include "media_msg.h"
#include "connect_rec_server.h"

#define TCP_MSG_TIME_OUT_SEC   			(5)		//tcp���ӳ�ʱʱ��,��λ����
#define TCP_CONNECT_PRIORITY 			sched_get_priority_max(SCHED_FIFO)
#define MSG_MAX_LEN 						(512)	//tcp��Ϣ�ĳ���
#define MAX_UDP_LEN  	     					(65536)	//udp������󳤶�
#define PORT_RNG							(20)	//ÿ��ͨ�������Ķ˿���
#define UDPBUFFERSZIE   					(655360) 	//����udp��buffer����
#define SEND_DATA_LEN					(8)			//���÷���ͨ���ź���Ҫ��·�������ݵĳ���
#define MEMORY_POOL_LEN					(1*1024*1024)		//�ڴ�صĳ���
#define MAX_CMD_MSG_LEN					(512)	//�����Ϣ����
#define MSG_HEADER_LEN					(8)		//��Ϣͷ����
#define UDP_PACKAGE_LEN					(1500 + MSG_HEADER_LEN)	//UDP���ݳ��ȼ���Ϣͷ�ĳ���.
#define MSG_SCREEN_DATA 					(7)		//��tcpͨѶ����Ϣͷ����Ƶ��־
#define MSG_AUDIO_DATA					(6)		//��tcpͨѶ����Ϣͷ����Ƶ��־
#define H264_CODEC_TYPE   				MAKEFOURCC('H','2','6','4')
#define AAC_CODEC_TYPE    					MAKEFOURCC('A','D','T','S')
#define LIST_ENQ							(10)	//������������

#define RECV_BROADCAST_DATE_NO      		(1)  // ����¼����Ϣ�����֪ ����¼����Ϣ
#define RECV_BROADCAST_DATE_OFF			(0) // ͬ�ϲ�����¼������
#define BEYONDSYS_MARK                                       (0xe7e7e7e7) // the info of the  beyondsys ��ʾ

#define PRINTF							 printf
connect_server_info_t g_connect_info[CHANNEL_CONNECT_MAX_NUM];


/*
****************************************************************************
**��������: live_rec_server_recv_data()
**��������: ����UDP����
**�������: connect_info -- Ϊ�ṹ��CONNECT_SERVER_INFO_T���͵Ĳ���
			��Ҫ�Ǵ���UDP���
			iSocket -- ���͸����ͽڵ�����ݵ�socket
**�������: ��	
**�������: -1 -- ��ʾû�����ݻ���udp�Ͽ���δ����,����0��ʾ���ݳ���.
**����ֵ:	
****************************************************************************
*/
static int32_t live_rec_server_recv_data(int32_t socket, int32_t recv_socke, int32_t index,
	int32_t id)
{
	int32_t 							recv_bytes 		= 0;
	int32_t							total_recv_byes       = 0;
	channel_info_t						channel_info;
         init_channel_infos(&channel_info);
	hdb_freame_head_t				*hdb_data		= NULL;
	int8_t							*ptr			= NULL;
	
	while(1)
	{
		ptr = r_malloc(UDP_PACKAGE_LEN);
		if(ptr == NULL)
		{
			printf("[live_rec_server_recv_data] pcalloc failed!\n");
			return -1;
		}
		r_memset(ptr,0,UDP_PACKAGE_LEN);
		recv_bytes = recv(recv_socke, ptr + MSG_HEADER_LEN, MAX_UDP_LEN, 0);

		if(recv_bytes < 1)
		{
			r_free(ptr);
			 break;
		}
		else
		{
			channel_info.m_data = (intptr_t *)ptr;
			channel_info.m_channel 	= id;
			channel_info.m_data_len	= (uint16_t)recv_bytes;
			hdb_data = (hdb_freame_head_t *)(channel_info.m_data + MSG_HEADER_LEN);
			hdb_data->m_id = index;
		
			if(hdb_data->m_data_codec == H264_CODEC_TYPE)
			{
				channel_info.m_msg_type = MSG_SCREEN_DATA;
			}
			else if(hdb_data->m_data_codec == AAC_CODEC_TYPE)
			{
				channel_info.m_msg_type = MSG_AUDIO_DATA;
			}

			#if RECV_REC_FILE			
			if(0 == channel_info.m_channel) {
				write_file("lives001.h264", channel_info.m_data + MSG_HEADER_LEN +FH_LEN, channel_info.m_data_len - FH_LEN);
			}
			#endif			
			r_send(socket,  &channel_info, sizeof(channel_info_t),0);
			total_recv_byes +=  recv_bytes;	
		}
	}
	return total_recv_byes;
}

/*
****************************************************************************
**��������: recv_long_tcp_data()
**��������: ��װ��TCP�������ݺ���,ȷ�����ݽ�������
**�������: socket -- tcp��sockfd
			len -- ��ȡ���ݵĳ���
**�������: buffer -- ��ȡ��������
**����ֵ:	-1 -- ����ʧ��; ����0Ϊ�������ݵĳ���.
****************************************************************************
*/
static int32_t recv_long_tcp_data(int32_t socket,void* buffer,const int32_t len)
{
	int32_t total_recv = 0;
	int32_t recv_len = 0;
	while(total_recv < len)
	{
		recv_len = r_recv(socket, buffer + total_recv, len - total_recv, 0);
		if(recv_len < 1)
		{	
			printf("[%s] ---recv_len is [%d]\n",__func__,recv_len);
			PRINTF("recv tcp data failed,rror message:%s \n",strerror(errno));
			if(recv_len == 0)
			{
				return SERVER_IS_CLOLSED;
			}
			return TRANSMISSION_DATA_ERR;
		}		
		total_recv += recv_len;
		if(total_recv == len)
		{
			break;
		}
	}
	return total_recv;
}

/*
****************************************************************************
**��������: send_long_tcp_data()
**��������: ��װ��TCP�ӷ��;ݺ���,ȷ�����ݷ�������
**�������: socket -- tcp��sockfd
			len -- ��ȡ���ݵĳ���
			buffer -- ��ȡ��������
**�������: ��
**����ֵ:	-1 -- ����ʧ��; ����0Ϊ�������ݵĳ���.
****************************************************************************
*/
static int32_t send_long_tcp_data(int32_t socket,void* buffer,const int32_t len)
{
	int32_t total_send = 0;
	int32_t send_len = 0;

	while(total_send < len)
	{
		send_len = r_send(socket, buffer + total_send, len - total_send, 0);
		if(send_len < 1)
		{
			PRINTF("send tcp data failed,rror message:%s \n",strerror(errno));
			return TRANSMISSION_DATA_ERR;
		}
		total_send += send_len;
		if(len == total_send)
		{
			break;
		}
	}
	return total_send;
}



/*
****************************************************************************
**��������: send_user_info_tcp_msg_to_server()
**��������: �����û���Ϣ��������.
**�������: socket -- tcp��sockfd
**�������: ��
**����ֵ:	-1 -- ����ʧ��; 0 -- �ɹ�
****************************************************************************
*/
static int32_t send_user_info_tcp_msg_to_server(int32_t socket)
{
	struct sockaddr_in 	lcadr;
	const int32_t 		nl = sizeof(msg_user_info_t) + sizeof(msg_header_t);
	uint8_t 			data[nl];
	msg_user_info_t 	user_info;
	msg_header_t 		msg_header;
	socklen_t 			ln = sizeof(lcadr);
	
	msg_header.m_ver 		= MSG_VER;
	msg_header.m_data 		= 0;
	msg_header.m_msg_type 	= MSG_TYPE_USEINFO;
	msg_header.m_len 		= r_htons(nl);
	
	r_memcpy(data,&msg_header,sizeof(msg_header_t));
	user_info.m_type 		= USER_RECPLY;
	user_info.m_local_ip 	= 0;
	
	if (r_getsockname(socket, (struct sockaddr*)&lcadr, &ln) == 0)
	{
		user_info.m_local_ip = lcadr.sin_addr.s_addr;
	}
	r_strcpy(user_info.m_user_name, "admin");
	r_memcpy(data + sizeof(msg_header_t),&user_info,sizeof(msg_user_info_t));
	if(send_long_tcp_data(socket,data,nl) < 1)
	{
		PRINTF("send user info to server failed \n");
		return TRANSMISSION_DATA_ERR;
	}
	return OPERATION_SUCC;
}

/*
****************************************************************************
**��������: send_user_id_pack()
**��������: ���Ͷ˿���Ϣ��������.
**�������: 
**�������: ��
**����ֵ:	-1 -- ����ʧ��; ����0Ϊ���ͳ���.
****************************************************************************
*/
static int32_t send_user_id_pack(const int32_t connect_mode,const int32_t user_id,const int32_t dx,
	const int32_t channel,const int8_t* server_ip,int32_t socket, int32_t udp_start_port)
{
	int32_t 				port 	= udp_start_port + dx + 100;
	int32_t 				temp = 0xe7e7e7e7;
	int32_t 				send_len;
	uint8_t 				buf[128];
	struct sockaddr_in 		serv_addr;
	int32_t     				unit_len = sizeof(int32_t);
	
	
	//powerlive���ͷ�ʽ
#if 0
	temp 	= 0x7e7e7e7e;
	r_memcpy(buf,&temp,4);
	temp 	= dx+1;
	r_memcpy(buf + 4,&temp,4);
	temp 	= user_id;
	r_memcpy(buf + 8,&temp,4);

	if(!connect_mode)
	{
		port += PORT_RNG * channel;
	}
	
	serv_addr.sin_family 	= AF_INET;
	serv_addr.sin_port 		= r_htons(port);
	
	r_inet_aton(server_ip,(struct in_addr *)&serv_addr.sin_addr);
	r_bzero(&(serv_addr.sin_zero),8);
	send_len = r_sendto(socket,  buf,  12, 0, (struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in));
#else

	//beyondsys���Ͳ�����
	temp = BEYONDSYS_MARK;
	r_memcpy(buf,&temp,unit_len);
	temp = dx+1;
	r_memcpy(buf + unit_len,&temp,unit_len);
	temp = user_id;
	r_memcpy(buf + unit_len*2,&temp,unit_len);

	temp = channel;
	r_memcpy(buf + unit_len*3 ,&temp,unit_len);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = r_htons(port);
	r_inet_aton(server_ip,(struct in_addr *)&serv_addr.sin_addr);
	r_bzero(&(serv_addr.sin_zero),8);
	send_len = r_sendto(socket,  buf,  unit_len*4 , 0, (struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in));
#endif

	if(send_len == -1)
	{
		PRINTF("errno = %d,strerror(errno = %s) \n",errno,strerror(errno));
	}
	return send_len;
}

/*
****************************************************************************
**��������: heart_thread()
**��������: ����ֱ��ͨ���̵߳Ľ���.
**�������: arg -- Ϊ�ṹ��CONNECT_SERVER_INFO_T���͵Ĳ���
**�������: ��
**����ֵ:	
****************************************************************************
*/
static void* heart_thread(void* arg)
{
	
	connect_server_info_t	*connect_info	= (connect_server_info_t *)arg;
	int32_t 				socket 			= connect_info->m_tcp_socket;
	msg_header_t 			msg_header;
	int32_t 				msg_len 		= sizeof(msg_header_t);
	int32_t 				send_num 		= 0;
	msg_header.m_ver 		= MSG_VER;
	msg_header.m_data 		= 0;
	msg_header.m_msg_type 	= MSG_NEW_HEART;
	msg_header.m_len 		= r_htons(msg_len);

	r_pthread_detach(r_pthread_self());
	
	while(connect_info->m_connect_status ==2)
	{
		socket 	= connect_info->m_tcp_socket;
		if(socket < 1)
		{
			r_sleep(1);
			continue;
		}
		send_num = send_long_tcp_data(socket,&msg_header,msg_len);
		if(send_num < 0)
		{
			printf("[%s]---heart thread exit  failed send = %d,errno = %d\n",__func__,send_num,errno);
			PRINTF("error MSg = %s \n",strerror(errno));
			printf("[%s]---broadcast_ip is in the hearthread-----------[%s]\n",__func__,connect_info->m_ip_addr);
		}
		r_sleep(1);
	}
	return (void *)OPERATION_SUCC;	
}

/*
****************************************************************************
**��������: tim_subtract()
**��������: ��UDP ������ʱ����������Ƿ���Ҫ�ϵ�
**�������: x----ǰ��¼ʱ��  y-------��ǰ��¼ʱ��
**�������:result -- ��ͨ��UDP �����ݵ�ʱ����
**����ֵ:	-1 -- ����ʧ��; 
****************************************************************************
*/
int32_t tim_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
         if ( x->tv_sec > y->tv_sec )
		return   -1;
        if ((x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec))
		return   -1;
        result->tv_sec = ( y->tv_sec-x->tv_sec );
        result->tv_usec = ( y->tv_usec-x->tv_usec );
        if (result->tv_usec<0)
       {
             result->tv_sec--;
             result->tv_usec+=1000000;
       }
       return   0;
}

/*
****************************************************************************
**��������: init_select_udp_array()
**��������: ��ʼ������¼�����ݵ�UDP ����
**�������: socket_channelID -- ����¼�����ݵ�UDP socket ����
**�������: NULL	
**����ֵ      : NULL
****************************************************************************
*/
void init_select_udp_array(udp_socket_channelID_info_t socket_channelID[])
{
	int32_t socket_channelID_loop = 0;
	for(socket_channelID_loop=0;socket_channelID_loop<CHANNEL_CONNECT_MAX_NUM;socket_channelID_loop++)
	{
		socket_channelID[socket_channelID_loop].m_channelID = -1;
		r_memset(socket_channelID[socket_channelID_loop].udp_socket,0,MAX_ROOM_BOX);
		socket_channelID[socket_channelID_loop].m_num_udp_socket = 0;
	}
	
}
/*
****************************************************************************
**��������: set_select_udp_array()
**��������: ���ý���¼�����ݵ�UDP ����
**�������: socket_channelID -- ����¼�����ݵ�UDP socket ����
			      connect_info_p  --- 	����ͨ������Ϣ�ṹ��
**�������: NULL	
**����ֵ      : �ɹ�����select������socket ���ֵ
****************************************************************************
*/
int32_t set_select_udp_array(udp_socket_channelID_info_t *socket_channelID,connect_server_info_t connect_info,fd_set *fdsr)
{
	int32_t socket_channelID_loop = 0;
	int32_t udp_socket_loop = 0;
	int32_t maxsock = 0;
	for(;socket_channelID_loop<CHANNEL_CONNECT_MAX_NUM;socket_channelID_loop++)
	{
				     
		if(socket_channelID[socket_channelID_loop].m_channelID != -1)
		{
			continue;
		}
		socket_channelID[socket_channelID_loop].m_channelID = connect_info.m_id;
		socket_channelID[socket_channelID_loop].m_num_udp_socket = connect_info.m_num_encode;
		for(udp_socket_loop = 0;udp_socket_loop < socket_channelID[socket_channelID_loop].m_num_udp_socket;udp_socket_loop++)
		{
			socket_channelID[socket_channelID_loop].udp_socket[udp_socket_loop] = connect_info.m_udp_socket[udp_socket_loop];
			FD_SET(socket_channelID[socket_channelID_loop].udp_socket[udp_socket_loop], fdsr);
			if(socket_channelID[socket_channelID_loop].udp_socket[udp_socket_loop] > maxsock)
			{
				maxsock = socket_channelID[socket_channelID_loop].udp_socket[udp_socket_loop];
			}
		}
		break;
	}
	
	#if RECV_SELECT_S
	printf("[%s]---local_id is [%d] is filled in the select_buf !\n",__func__,connect_info.m_id);
	#endif
	return maxsock;
}

/*
****************************************************************************
**��������: deal_select_udp_array_timeout()
**��������: ����UDP ��������ʱ����
**�������: socket_channelID -- ����¼�����ݵ�UDP socket ����
			      connect_info_p  --- ȫ��ͨ������ָ��
**�������: NULL	
**����ֵ      : �ɹ�����0	
****************************************************************************
*/
int32_t deal_select_udp_array_timeout(udp_socket_channelID_info_t *socket_channelID,connect_server_info_t * connect_info_p)
{
	int32_t i_loop = 0;
	//��ʱ���� ��Ϊ��������¼�������� UDP_NO_DATA_MAX
	for( i_loop = 0; i_loop<CHANNEL_CONNECT_MAX_NUM;i_loop++)
	{
		if(socket_channelID[i_loop].m_channelID != -1)
		{	
			#if RECV_SELECT_S
			printf("[%s]----[%d]----[%d] is select timeout\n",connect_info_p[(socket_channelID[i_loop].m_channelID)].time_udp_no_data.tv_sec ,connect_info_p[(socket_channelID[i_loop].m_channelID)].time_udp_no_data.tv_usec);
			#endif
			if(broadcast_udp_check_online(&connect_info_p[(socket_channelID[i_loop].m_channelID)],socket_channelID[i_loop].m_channelID) == -1)
			{
				printf("[%s]---broadcast_udp_check_online is error in the select is doing!\n",__func__);
			}				
		}
	}
	return OPERATION_SUCC;
}


/*
****************************************************************************
**��������: deal_select_udp_array()
**��������: ��ʼ��ȫ��ֱ����㱾��ȫ��ͨ������
**�������:  socket_channelID -- UDP socket ����
			       connect_info_p   --- ����ȫ������ͨ������Ϣ�ṹ��
			      fdsr ----select ����
			       msg_data_socket ---����socket 
**�������:  NULL
**����ֵ       :  NULL
****************************************************************************
*/
int32_t deal_select_udp_array(udp_socket_channelID_info_t *socket_channelID,connect_server_info_t * connect_info_p, fd_set *fdsr, int32_t msg_data_socket )
{
	int32_t udp_socket_loop 				= 0;
	int32_t live_recv_code					= 0;
	int32_t channel_id 						= 0 ;
	int32_t online 							= 0;
	int32_t  socket_channelID_loop 			= 0;
	int32_t recv_socket						= 0;
	for(socket_channelID_loop=0;socket_channelID_loop<CHANNEL_CONNECT_MAX_NUM;socket_channelID_loop++)
	{
		online = 0;
		if(socket_channelID[socket_channelID_loop].m_channelID == -1)
		{
			continue;
		}
		channel_id = socket_channelID[socket_channelID_loop].m_channelID;
		for(udp_socket_loop = 0;udp_socket_loop<socket_channelID[socket_channelID_loop].m_num_udp_socket;udp_socket_loop++)
		{
			 live_recv_code = 0; 
			if (FD_ISSET(socket_channelID[socket_channelID_loop].udp_socket[udp_socket_loop], fdsr)) 
			{
				#if RECV_UDPDATA_S
				printf("[%s]---channelID [%d]---index[%d] to call [live_rec_server_recv_data]!\n",__func__,channel_id,udp_socket_loop);
				#endif
				recv_socket = socket_channelID[socket_channelID_loop].udp_socket[udp_socket_loop];
				live_rec_server_recv_data(msg_data_socket, recv_socket,udp_socket_loop,channel_id);//
				online = 1;
			}
		}
		if(online == 0 )
		{
			if(broadcast_udp_check_online(&connect_info_p[channel_id],channel_id) == -1)
			{							
				printf("[%s]---broad_ip [%s]---broad_id[%d]---local_id[%d] is error in the [tim_subtract]\n",__func__,connect_info_p[channel_id].m_ip_addr,connect_info_p[channel_id].m_channel,channel_id);
				return -1;
			}					
		}
		else
		{
			if(connect_info_p[channel_id].time_udp_no_data.tv_sec != 0 ||connect_info_p[channel_id].time_udp_no_data.tv_usec != 0)
			{
				connect_info_p[channel_id].time_udp_no_data.tv_sec = 0;
				connect_info_p[channel_id].time_udp_no_data.tv_usec = 0;
			}
		}
	}
	return 0;
}

/*
****************************************************************************
**��������: get_broadcast_headmsg_tcp()
**��������: ����¼�����͵���Ϣͷ��Ϣ
**�������:  connect_info --	����ͨ������Ϣ�ṹ��	
			       user_id ---???
**�������:  msg_data---¼����Ϣ��
**����ֵ       :  (-8) ¼���ر�(0) �����ɹ�(>0)¼����Ϣ����
****************************************************************************
*/
int32_t get_broadcast_headmsg_tcp(connect_server_info_t *connect_info, int32_t *user_id, int8_t *msg_data)
{
	int32_t real_recv_len = 0;
	int32_t msg_data_len = 0;
	int32_t recv_len     = sizeof(msg_header_t);
	int32_t tcp_socket = connect_info->m_tcp_socket ;
	msg_header_t msg_header;
	r_memset(&msg_header,0,sizeof(msg_header_t));
	
	real_recv_len = recv_long_tcp_data(tcp_socket, &msg_header, recv_len);
	if(real_recv_len < recv_len)
	{
		printf("[%s]---the recv_ip [%s]---recv_id [%d]---local_id [%d]recv_long_tcp_data len is [%d] \n",__func__,connect_info->m_ip_addr,connect_info->m_channel,connect_info->m_id,real_recv_len);
		if(real_recv_len == SERVER_IS_CLOLSED)
		{
			printf("[%s]---broad_cast is closed!\n",__func__);
			connect_info->m_connect_status = BC_CONNECT_CLOSE;
			connect_info->m_bt_connect = TCP_BT_RECONNECT;
			return SERVER_IS_CLOLSED;
		}
		 r_usleep(10000);
		 return OPERATION_SUCC;
	}
	msg_data_len = r_ntohs(msg_header.m_len) - recv_len;

	if(((msg_header.m_msg_type == MSG_FIRE_WALL) || 
	(msg_header.m_msg_type ==MSG_TYPE_FIREWALL)) && (msg_data_len >= 4))
	{
		if(recv_long_tcp_data(tcp_socket,user_id,4) < 4)
		{
			printf("[%s]---[recv_long_tcp_data] is failed \n",__func__);
		}
		msg_data_len -= 4;
	}

	if(msg_data_len > 0)
	{	
		int32_t factrecvlen = 0;
		if(recv_len > MSG_MAX_LEN)
		{
			recv_len = MSG_MAX_LEN;
		}
		else
		{
			recv_len = msg_data_len;
		}
		r_memset(msg_data, 0, MSG_MAX_LEN);
		factrecvlen = recv_long_tcp_data(tcp_socket,msg_data,recv_len);
		if(factrecvlen < recv_len) 
		{
			return OPERATION_SUCC;
		}
	}
	return msg_header.m_msg_type;
}

/*
****************************************************************************
**��������: deal_broadcast_msg_tcp()
**��������: ����ֱ�������ܵ���¼����Ϣ
**�������:  m_msg_type --- ¼����Ϣ����
			      connect_info ----  ����ͨ������Ϣ�ṹ��	
			      user_id ---???
**�������:  NULL
**����ֵ       :  (-4)����ͨ��ʧ�� (-3)�����߳�����ʧ�� (0) �����ɹ�
****************************************************************************
*/
int32_t deal_broadcast_msg_tcp(int32_t m_msg_type , int32_t *udp_socket ,connect_server_info_t *connect_info, int32_t  user_id)
{
	int32_t tcp_socket = connect_info->m_tcp_socket ;
	int32_t index = 0;
	switch(m_msg_type)
	{
	case MSG_FIRE_WALL:		
	case MSG_TYPE_FIREWALL:	
		/*��͸����ǽ*/
		send_user_info_tcp_msg_to_server(tcp_socket);
		if(user_id != 0)
		{
			for(index = 0; index < connect_info->m_num_encode; index++)
			{
				//0��ʾ����,1��ʾ�鲥
				if(send_user_id_pack(0,user_id,index,connect_info->m_channel,connect_info->m_ip_addr,
				udp_socket[index], connect_info->m_udp_start_port) < 1)
				{
					printf("[%s]--- [send_user_id_pack] is Failed \n",__func__);
					return TRANSMISSION_DATA_ERR;
				}	
				connect_info->m_udp_socket[index] = udp_socket[index];	
				connect_info->time_udp_no_data.tv_sec = 0;
				connect_info->time_udp_no_data.tv_usec = 0;
			}
			connect_info->m_connect_status = BC_CONNECT_OK;
			printf("[%s]---broadcast of ip[%s]------channelid is [%d]-----rec_channelid is [%d] connect is ok!\n",__func__,connect_info->m_ip_addr,connect_info->m_id,connect_info->m_channel);
			connect_info->m_bt_connect = 0 ;
			connect_info->m_state_change = HTTP_CON_STATUS_CHANGED;
			// ���������������߳�
			if (r_pthread_create(&connect_info->m_heart_thread, NULL,
			heart_thread, connect_info)) 
			{ 
				printf("[%s]---Failed to create HeartThread video thread\n",__func__);		
				return CREATE_THREAD_ERR;		
			}
		}break;
	default:
		break;
	}
	return OPERATION_SUCC;
}

/*
****************************************************************************
**��������: deal_select_msg_tcp()
**��������: ��select ģ�ʹ���Ľ���¼����Ϣ������
**�������: udp_socket   -- ֱ������udp_socket
			      connect_info -- ����ͨ������Ϣ�ṹ��			      
**�������: msg_data  ---- ¼����Ϣ��
			      msg_header ---¼����Ϣͷ
**����ֵ:	(1) select ѭ�����ܴ������(-1)����ʧ������ѭ��(0)�ɹ�����
****************************************************************************
*/
int32_t deal_select_msg_tcp(connect_server_info_t *connect_info,int32_t *udp_socket, int8_t *msg_data,msg_header_t *msg_header)
{
	int32_t user_id = 0;
	int32_t return_code = 0;
	msg_header->m_msg_type = get_broadcast_headmsg_tcp(connect_info,&user_id,msg_data);
	
	switch(msg_header->m_msg_type)
	{
	case SERVER_IS_CLOLSED:
		return OPERATION_SUCC;
	case OPERATION_SUCC:
		return OPERATION_CONTINUE;
	default:
		{
			return_code = deal_broadcast_msg_tcp(msg_header->m_msg_type , udp_socket, connect_info, user_id);
			if(return_code != 0)		
				return OPERATION_ERR;
			else
				return OPERATION_CONTINUE;
		}
	}
}

/*
****************************************************************************
**��������: g_connect_info_init()
**��������: ��ʼ��ȫ��ֱ����㱾��ȫ��ͨ������
**�������:  NULL	
**�������:  NULL
**����ֵ       :  NULL
****************************************************************************
*/
void g_connect_info_init()
{
	int i_loop = 0;
	for(; i_loop <CHANNEL_CONNECT_MAX_NUM;i_loop ++)
	{
		g_connect_info[i_loop].m_id = -1;
		g_connect_info[i_loop].m_state_change = 0;
	}
}

/*
****************************************************************************
**��������: broadcast_udp_check_online()
**��������: ����¼������������ģʽ�ĳ�ʱ����
**�������:  id -- ͨ���ı������
**�������: connect_info -- ����ͨ������Ϣ�ṹ��
**����ֵ       : (-1)����ʧ�� (0) ����ɹ�
****************************************************************************
*/
int32_t broadcast_udp_check_online(connect_server_info_t *connect_info,int32_t id )
{
	struct  timeval  time_now ;
	time_now.tv_sec = 0;
	time_now.tv_usec =0;
	struct  timeval  time_old;
	time_old.tv_sec = 0;
	time_old.tv_usec = 0 ;
	struct  timeval delta_time ;
	delta_time.tv_sec = 0;
	delta_time.tv_usec = 0;
	
	gettimeofday(&time_now,0);
	time_old.tv_sec = connect_info->time_udp_no_data.tv_sec;
	time_old.tv_usec = connect_info->time_udp_no_data.tv_usec;
	if(time_old.tv_sec == 0 && time_old.tv_usec == 0)
	{
		connect_info->time_udp_no_data.tv_sec =time_now.tv_sec;
		connect_info->time_udp_no_data.tv_usec =time_now.tv_usec;
		return OPERATION_SUCC;
	}
	if(tim_subtract(&delta_time,&time_old,&time_now) == -1)
	{
		printf("[%s]---[tim_subtract] is error!\n",__func__);
		connect_info->m_connect_status =BC_CONNECT_CLOSE;
		connect_info->m_state_change = HTTP_CON_STATUS_CHANGED;
		connect_info->m_bt_connect = TCP_BT_RECONNECT;
		return OPERATION_ERR;
	}
	
	if(delta_time.tv_sec > 3)
	{		
		connect_info->m_connect_status = BC_CONNECT_CLOSE;
		connect_info->m_state_change = HTTP_CON_STATUS_CHANGED;
		connect_info->m_bt_connect = TCP_BT_RECONNECT;
		#if RECV_SELECT_S
		printf("[%s]---the local_id [%d] ---broadcast_ip [%s]--- broadcast_id [%d] is Disconnection!\n",__func__,id,connect_info->m_channel,connect_info->m_ip_addr);
		#endif
	}
	return OPERATION_SUCC;
}


/*
****************************************************************************
**��������: open_udp_socket()
**��������: �򿪽�����Ƶ����Ƶ���ݵ�UDP�˿�
**�������: index -- ��ʾ��ͨ���ڵĵڼ���������;
			live_base_port -- �˿ں�
			channel_id -- ͨ�����
**�������: ��
**����ֵ:	-1 -- ��ʧ��; ����0Ϊudp��socket
****************************************************************************
*/
static int32_t	open_udp_socket(int32_t index, int32_t live_base_port, int32_t channel_id)
{
	int32_t 					opt 						= 1;
	int32_t  				 	udp_socket					= 0;
	struct sockaddr_in 	 		local_addr;  
	struct timeval 				tv;
	int32_t 					recv_buf 					= UDPBUFFERSZIE;

	tv.tv_sec 	= TCP_MSG_TIME_OUT_SEC;
    	tv.tv_usec 	= 500;
   	 udp_socket = r_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
    	if(udp_socket < 1)
   	 {
    	   	return OPEN_DEV_ERR;
  	  }
		
#if 1
	r_bzero(&local_addr, sizeof(local_addr));
	local_addr.sin_family 		= AF_INET;
	local_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);

	local_addr.sin_port = r_htons(index + live_base_port + channel_id *20);
	r_setsockopt(udp_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)); 

	//��IP��ַ�Ͷ˿���Ϣ��socket��
	if(bind(udp_socket, (struct sockaddr *)&local_addr, sizeof(local_addr)) == -1)
	{
		return OPEN_DEV_ERR;
	}
	fcntl(udp_socket, F_SETFL, O_NONBLOCK);		//��������Ϊ������.
#else
	//��ϵͳ�Զ��󶨶˿�
	fcntl(iUdpSocket, F_SETFL, O_NONBLOCK);		//��������Ϊ������.
#endif	
	r_setsockopt(udp_socket,SOL_SOCKET,SO_RCVBUF,(const int8_t*)&recv_buf,sizeof(int32_t));	

	return udp_socket;
}

/*
****************************************************************************
**��������: connect_live_server()
**��������: ����tcp����
**�������: ip_addr -- ��ʾҪ���ӵ�IP��ַ;
			channel -- Ҫ���ӵ�ͨ����,��0��ʼ
			tcp_start_port -- tcp������ʼ�˿ں�
**�������: ��
**����ֵ:	OPERATION_ERR_1 -- ����ʧ��; ����0Ϊtcp��socket
****************************************************************************
*/
static int32_t connect_live_server(const int8_t* ip_addr,int32_t channel,int32_t tcp_start_port)
{
	int32_t 				tcp_socket		= 0;
	struct timeval 			tv;
	struct sockaddr_in 		serv_addr;
		
	tcp_socket = r_socket(PF_INET, SOCK_STREAM, 0);
	if(tcp_socket < 1)
	{
		printf("[%s]---",__func__);
		PRINTF("Create Socket failed,error message:%s \n",strerror(errno));
		return OPEN_DEV_ERR;
	}
	
	serv_addr.sin_family 	= AF_INET;
	serv_addr.sin_port 		= r_htons(tcp_start_port);//+ channel);
	r_inet_aton(ip_addr,(struct in_addr *)&serv_addr.sin_addr);
	r_bzero(&(serv_addr.sin_zero),8);
	
    	tv.tv_sec 	= TCP_MSG_TIME_OUT_SEC;
    	tv.tv_usec 	= 0;
	
	r_setsockopt(tcp_socket,SOL_SOCKET,SO_SNDTIMEO,(int8_t *)&tv,sizeof(struct timeval));

	if (r_connect(tcp_socket, (struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1) 
	{
		printf("[%s]---recv_ip[%s]---recv_id[%d] is connect failed!----",__func__,ip_addr,channel);
		PRINTF("error message:%s \n",strerror(errno));
		r_close(tcp_socket);
		tcp_socket= 0;
		return TRANSMISSION_DATA_ERR;
	}
	return tcp_socket;

}

/*
****************************************************************************
**��������: send_tcp_password()
**��������: �����������������֤
**�������: socket -- tcp��socket;
			passwd -- ��������
**�������: ��
**����ֵ:	OPERATION_ERR_1 -- ����ʧ��; OPERATION_SUCC -- ���ͳɹ�
****************************************************************************
*/
static int32_t send_tcp_password(int32_t socket,int8_t *passwd)
{
	msg_header_t 		msg_header;
	int32_t				send_len			= 0;
	net_user_info_t nu;

	pack_unite_user_info(&nu);
	//����������֤
	msg_header.m_ver 		= MSG_VER;
	msg_header.m_data 		= 0;
	msg_header.m_msg_type 	= MSG_TYPE_LOGIN;
	msg_header.m_len 		= r_htons(sizeof(msg_header_t) + sizeof(net_user_info_t));
	send_len = sizeof(msg_header_t);
	
	if(send_long_tcp_data(socket,&msg_header,send_len) < send_len)
	{
		PRINTF("Send Message Header to server failed error message:%s \n",strerror(errno));	
		r_close(socket);
		return TRANSMISSION_DATA_ERR;
	}
	send_len = sizeof(net_user_info_t);
	if(send_long_tcp_data(socket,(void*)&nu,send_len) < send_len)	
	{		
		PRINTF("Send Password to server failed error message:%s \n",strerror(errno));		
		r_close(socket);		
		return TRANSMISSION_DATA_ERR;		
	}
	return OPERATION_SUCC;

}

/*
****************************************************************************
**��������: recv_tcp_password()
**��������: ���շ��������ص�������֤���.
**�������: socket -- tcp��socket;
**�������: ��
**����ֵ:	OPERATION_ERR_1 -- ����ʧ��,ֱ���˳�; OPERATION_ERR_1 -- ��Ϣ��
			�Ͳ�ƥ��,���½���; OPERATION_SUCC -- ��֤�ɹ�
****************************************************************************
*/
static int32_t recv_tcp_password(int32_t socket)
{
	msg_header_t 		msg_header;
	int32_t 			recv_len  			= 0;
	int32_t login = -1;
	recv_len	= sizeof(msg_header_t);
	if(recv_long_tcp_data(socket,&msg_header,recv_len) < recv_len)	
	{		
		printf("[%s]---recv_tcp_password is call the recv_long_tcp_data!\n",__func__);
		PRINTF("Recive Message header from server failed error message:%s \n",strerror(errno));
		return TRANSMISSION_DATA_ERR;
	}
	recv_len = r_ntohs(msg_header.m_len) - sizeof(msg_header_t);
	if(recv_long_tcp_data(socket,(void *)&login, recv_len) < recv_len)
	{
		PRINTF("Recive Message from server failed error message:%s \n",strerror(errno));
		return TRANSMISSION_DATA_ERR;
	}
	if(msg_header.m_msg_type != MSG_TYPE_LOGIN)
	{	
		return MESSAGE_TYPE_ERR;
	}
	//������֤�жϱ�־,Ϊ0��ɹ�,����ʧ��
	if(msg_header.m_data != OPERATION_SUCC) 
	{	
		return PASSWD_VERIFICATION_ERR;
    	}

	if(MSG_TYPE_LOGIN != login) {
		return PASSWD_VERIFICATION_ERR;
	}
	return OPERATION_SUCC;

}


/*
****************************************************************************
**��������: request_media_data()
**��������: ����ý������
**�������: socket -- tcp��socket;
			channel��-- ͨ����
**�������: ��
**����ֵ:	OPERATION_ERR_1 -- ��������ʧ��; OPERATION_SUCC -- ��֤�ɹ�
****************************************************************************
*/
static int32_t request_media_data(int32_t socket, int32_t channel)
{
	
	int8_t 			data[SEND_DATA_LEN] 	= {0};
	int32_t 			temp 					= 0;
	int8_t 			test 					= 1;
	int32_t 			send_len 				= 0;
	msg_header_t 	msg_header;	
	int32_t			unit_len				=sizeof(int8_t);

	//ǰ�ĸ��ֽ���ͨ����,�Ǵ�0��ʼ,������ֽ�Ϊ����ʾ����ý�����ݣ�Ϊ��������
	temp = channel;
	r_memcpy(data,&temp,4);

	test = RECV_BROADCAST_DATE_NO;
	r_memcpy(data + 4,&test,unit_len);	
	test = RECV_BROADCAST_DATE_NO;
	r_memcpy(data + 5,&test,unit_len);
	test = RECV_BROADCAST_DATE_NO;
	r_memcpy(data + 6,&test,unit_len);	
	test = RECV_BROADCAST_DATE_NO;
	r_memcpy(data + 7,&test,unit_len);
	
	msg_header.m_ver 	= MSG_VER;
	msg_header.m_data 	= 0;
	msg_header.m_msg_type = MSG_TYPE_DATAREQ;
	msg_header.m_len 	= r_htons(sizeof(msg_header_t) + SEND_DATA_LEN);
	
	send_len	= sizeof(msg_header_t);
	
	if(send_long_tcp_data(socket,&msg_header,sizeof(msg_header_t)) < send_len)
	{
		PRINTF("Send Message Header to server failed error message:%s \n",strerror(errno));	
		return -1;
	}	
	send_len = SEND_DATA_LEN;
	if(send_long_tcp_data(socket,&data,send_len) < send_len)
	{
		PRINTF("Send Message Header to server failed error message:%s \n",strerror(errno));	
		return -1;
	}	
	return OPERATION_SUCC;
}

/*
****************************************************************************
**��������: recv_tcp_msg()
**��������: tcp��Ϣ������
**�������: udp_socket -- Ҫ��͸����ǽ��udp��socket;
			connect_info -- �û����εĽṹ��,�����ոմ�����tcp���.
**�������: ��
**����ֵ:	OPERATION_ERR_1 -- ��͸����ǽʧ���˳�; OPERATION_SUCC -- �����˳�
****************************************************************************
*/
static int32_t recv_tcp_msg(connect_server_info_t *connect_info,int32_t *udp_socket)
{
	int8_t 				msg_data[MSG_MAX_LEN] 		= {0};
	int32_t				tcp_socket					= 0;
	msg_header_t 		msg_header;
	int32_t 				select_recode 					= 0;

	struct timeval tv;
	fd_set fdsr;
	tv.tv_sec 	= 1;
	tv.tv_usec =0;
	
	while(connect_info->m_connect_status )
	{
		tcp_socket = connect_info->m_tcp_socket;
		tv.tv_sec = 1;
		tv.tv_usec =0;
		FD_ZERO(&fdsr);
		FD_SET(tcp_socket,&fdsr);

		select_recode = select(tcp_socket + 1, &fdsr, NULL, NULL, &tv);
		switch(select_recode)
		{
		case -1:
			{				
				if(errno == EINTR)
					printf("[%s]+++select  error is EINTR!\n",__func__);
				else{
					printf("[%s]----there is a error!",__func__);
					fprintf(stderr,"select error is---:[%s]\n",strerror(errno));
				}
				continue;
			}break;
		case 0:
			{
				continue;
			}break;
		default:
			{
				if (FD_ISSET(tcp_socket,&fdsr))
				{
					if(deal_select_msg_tcp(connect_info,udp_socket,msg_data,&msg_header) != OPERATION_CONTINUE)
					{
						return OPERATION_SUCC;
					}
				}
			}break;		
		}
	}
	return OPERATION_SUCC;
}

/*
****************************************************************************
**��������: create_connect_thread()
**��������: ����ֱ��ͨ���̵߳Ľ���.
**�������: arg -- Ϊ�ṹ��CONNECT_SERVER_INFO_T���͵Ĳ���
**�������: ��
**����ֵ:	
****************************************************************************
*/
static void* create_connect_thread(void* arg)
{

	connect_server_info_t	*connect_info	= (connect_server_info_t *)arg;
	int32_t 				tcp_socket		= 0;
	int32_t					ret				= 0;
	int32_t					num				= 0;
	int32_t					index			= 0;
	int32_t					udp_socket[MAX_ROOM_BOX];		//����udp���ӵ�socket.						

RECONNECT:	
	//����tcp����
	tcp_socket = connect_live_server(connect_info->m_ip_addr, connect_info->m_channel, connect_info->m_tcp_start_port);
	if(tcp_socket < 1)
	{
		if(connect_info->m_connect_status)
		{
			
			printf("[%s]---goto reconnect!\n",__func__);
			r_sleep(2);
			goto RECONNECT;		
		}
		else
		{
			goto EXIT;
		}
	}
	//����������֤��Ϣ
	if(send_tcp_password(tcp_socket, (int8_t *)connect_info->m_passwd_info) < 0)
	{
		if(connect_info->m_connect_status)
		{
			r_close(tcp_socket);
			r_sleep(1);
			goto RECONNECT;
		}
		else
		{
			goto EXIT;
		}
	}
	for(num = 0; num < 3; num++)
	{
		//����������֤������Ϣ
		ret = recv_tcp_password(tcp_socket);
		if(ret == TRANSMISSION_DATA_ERR)
		{
			goto EXIT;
		}
		else if(ret == MESSAGE_TYPE_ERR)
		{
			continue;
		}
		break;
	}
	ret = request_media_data(tcp_socket, connect_info->m_channel);
	if(ret != OPERATION_SUCC)
	{
		goto EXIT;
	}
	
	connect_info->m_tcp_socket = tcp_socket;
	
	for(index = 0; index < connect_info->m_num_encode; index++)
	{
		udp_socket[index] = open_udp_socket(index, connect_info->m_udp_start_port, connect_info->m_id);
	}
	if(recv_tcp_msg(connect_info,udp_socket))
	{
		 goto EXIT;
	}
EXIT:
	//�ر�tcp��socket
	if(connect_info->m_tcp_socket > 0)
	{	
		r_close(connect_info->m_tcp_socket);
		connect_info->m_tcp_socket = 0;
	}
	//�ر�udp��socket
	for(index = 0; index < connect_info->m_num_encode; index++)
	{
		if(connect_info->m_udp_socket[index] > 0)
		{
			r_close(connect_info->m_udp_socket[index]);
			connect_info->m_udp_socket[index] =0;
		}
		connect_info->m_udp_socket[index] = 0;
	}
	if(connect_info->m_bt_connect ==TCP_BT_RECONNECT)
	{
		connect_info->m_connect_status = BC_CONNECTING;
		printf("[%s]---braod_ip [%s]---braod_id [%d]---local_id [%d]---reconnect broadcast!\n",__func__,connect_info->m_ip_addr,connect_info->m_channel,connect_info->m_id);
		r_sleep(2);
		goto RECONNECT;
	}
	r_pthread_detach(r_pthread_self());
	return (void *)OPERATION_SUCC;
	
}

/*
*****************************************************************************
**��������: live_rec_server_init()
**��������: ��ʼ��Ҫ���εĽṹ��,��Ϊ����ʱ�Ĳο�����.
**�������: connect_info -- Ҫ����ȥ�Ľṹ��
**�������: ��
**����ֵ:	OPERATION_SUCC: �ɹ�����.
*****************************************************************************
*/
int32_t live_rec_server_init(connect_server_info_t *connect_info, web_cmd_parse_info_t *cmd_info)
{
	r_memset(connect_info, 0, sizeof(connect_server_info_t));
	r_strcpy(connect_info->m_ip_addr,cmd_info->rec_ip_addr);		//���ӷ�����IP
	r_strcpy(connect_info->m_passwd_info,"reachserver");
	connect_info->m_channel = cmd_info->rec_channel;				//���ӵ�ͨ����
	connect_info->m_id	= cmd_info->channel_id;				
	connect_info->m_udp_start_port = UDP_START_PORT;			//udp������ʼ�˿�,��������beyonsys,����Ҫ��100
	connect_info->m_tcp_start_port = TCP_START_PORT;			//tcp������ʼ�˿�
	connect_info->m_num_encode = 6;							//ÿ��ͨ������������,���ֵΪ6
	connect_info->m_reconnect_flag = 0;							//������־.1Ϊ����,0Ϊ������.
	connect_info->m_connect_status = BC_CONNECTING;				//���ӷ�����״̬,1Ϊ����,0Ϊ�˳�����
	connect_info->m_bt_connect = 0;
	connect_info->time_udp_no_data.tv_sec = 0;
	connect_info->time_udp_no_data.tv_usec = 0;
	return OPERATION_SUCC;
}
/*
*****************************************************************************
**��������: live_rec_server_connect()
**��������: ���ӷ�����.
**�������: pConnectServerInfo --Ҫ����ȥ�Ľṹ��
**�������: ��
**����ֵ:	0: �ɹ�����.
*****************************************************************************
*/
int32_t live_rec_server_connect(connect_server_info_t *connect_info)
{
	//����tcp�߳�
	if (r_pthread_create(&connect_info->m_connect_thread, NULL, 
		create_connect_thread, connect_info)) 
	{
		PRINTF("Failed to create connect thread thread = %s\n",strerror(errno));
		return CREATE_THREAD_ERR;	
	}		
	return OPERATION_SUCC;
}

/*
*****************************************************************************
**��������: live_rec_server_exit()
**��������: �˳����ӷ�����.
**�������: connect_info -- Ҫ����ȥ�Ľṹ��
**�������: ��
**����ֵ:	0: �ɹ�����.
*****************************************************************************
*/
int32_t live_rec_server_exit(connect_server_info_t *connect_info)
{
	connect_info->m_connect_status = BC_CONNECT_CLOSE;
	connect_info->m_state_change = HTTP_CON_STATUS_CHANGED;
	connect_info->m_bt_connect = 0;
	return OPERATION_SUCC;
}

/*
*****************************************************************************
**��������: create_msg_data_thread()
**��������: ���շ����������ݲ������ݵ�ַת�������Ͷ�
**�������: arg -- �̲߳���
**�������: ��
**����ֵ:	0: �ɹ�����.
*****************************************************************************
*/
static void* create_msg_data_thread(void* arg)
{
	udp_socket_channelID_info_t socket_channelID[CHANNEL_CONNECT_MAX_NUM];
	int32_t 					connect_state_loop = 0;
	int32_t					socket_channelID_loop = 0;
 	int32_t					select_recode =0;
	int8_t 					temp[MAX_CMD_MSG_LEN];
	channel_info_t				channel_info;
	int32_t					cur_con_status =0;
	int32_t 					m_old_con_status[100] ;
	r_memset(&m_old_con_status,0,100);
	
	int32_t maxsock = 0;
	int32_t temp_maxsock = 0;
	fd_set fdsr;
	struct timeval tv;
	
	connect_server_info_t * connect_info = (connect_server_info_t *)arg;
	int32_t						msg_data_socket	= 0;
	int32_t 						ret 			= 0;
	struct sockaddr_un				serv_addr;	
	
	r_pthread_detach(r_pthread_self());

	msg_data_socket = r_socket(AF_LOCAL, SOCK_STREAM, 0);
	r_bzero(&serv_addr, sizeof(struct sockaddr_un));
	serv_addr.sun_family = AF_LOCAL;
	r_strcpy(serv_addr.sun_path, LOACAL_DATA_MSG);

	while(1)
	{
		ret = r_connect(msg_data_socket, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_un));
		if(ret > -1)
		{
			printf("[%s]------------[ret > -1]------------\n",__func__);
			break;
		}
		r_usleep(500000);
	}
	while(1)
	{		
		maxsock = 0;
		tv.tv_sec = 0;
		tv.tv_usec =50000;
		init_select_udp_array(socket_channelID);
		socket_channelID_loop=0;
		FD_ZERO(&fdsr);
		for(connect_state_loop=0;connect_state_loop<CHANNEL_CONNECT_MAX_NUM;connect_state_loop++)
		{
			init_channel_infos(&channel_info);
			cur_con_status = connect_info[connect_state_loop].m_connect_status;
			if(cur_con_status == BC_CONNECTING)
			{
				continue;
			}
			// �趨select ����
			if(cur_con_status == BC_CONNECT_OK)
			{
				temp_maxsock = set_select_udp_array(socket_channelID,connect_info[connect_state_loop],&fdsr);
				maxsock = (temp_maxsock > maxsock )? temp_maxsock : maxsock ;
				#if RECV_SELECT_S
				printf("[%s]---local_id is [%d] is filled in the select_buf !\n",__func__,connect_info[connect_state_loop].m_id);
				#endif
			}
			// �ж�ֱ�����ͨ��״̬���
			if(cur_con_status ==BC_CONNECT_OK && m_old_con_status[connect_state_loop] ==BC_CONNECT_CLOSE)
			{
				m_old_con_status[connect_state_loop] = cur_con_status;
			}
			if(cur_con_status == BC_CONNECT_CLOSE && m_old_con_status[connect_state_loop] == BC_CONNECT_OK)
			{
				channel_info.m_msg_flag = 1;
				m_old_con_status[connect_state_loop] = cur_con_status;
				r_memcpy(temp, &channel_info,sizeof(channel_info_t));
				r_send(msg_data_socket, temp, sizeof(channel_info_t),0);
			}
		}
		if(maxsock == 0 )
		{
			r_sleep(1);
			continue;
		}
		#if RECV_SELECT_S
		printf("[%s]---maxsock is ---  [%d]\n",__func__,maxsock);
		#endif
		select_recode = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		switch(select_recode)
		{
			case -1:
				printf("[%s]---select is error!\n",__func__);
			break;
			case 0:
				deal_select_udp_array_timeout(socket_channelID,connect_info);
			break;
			default:
			{
				deal_select_udp_array(socket_channelID,connect_info,&fdsr,msg_data_socket);
			}break;
		}
	}
	return NULL;
}

int32_t create_recv_data_client(connect_server_info_t *connect_info)
{
	pthread_t   msg_local_data_thread;
	if (r_pthread_create(&msg_local_data_thread, NULL, 
		create_msg_data_thread, connect_info)) 
	{
		PRINTF("[%s]---Failed to create connect thread thread = %s\n",__func__,strerror(errno));
		return CREATE_THREAD_ERR;	
	}
	return OPERATION_SUCC;
}

int32_t	init_recv_data_thread(void)
{
	create_recv_data_client(g_connect_info);
	return OPERATION_SUCC;
}



