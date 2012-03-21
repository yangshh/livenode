
#include "reach_os.h"
#include "reach_socket.h"
#include "client.h"
#include "user.h"
#include "local_data.h"
#include "media_msg.h"
#include "key.h"
#include "stdint.h"

#define	FALSE	0
#define	TRUE	1

typedef struct manager_users  man_user_t;
struct manager_users {
	int32_t	user_total;
	int (*get_user_total)(man_user_t *);
	void (*add_user)(man_user_t *);
	void (*del_user)(man_user_t *);
};


int8_t is_response_msg(user_t *u)
{
	int8_t send_cmd_msg		= FALSE;

	/*没有信令需要回复*/
	if(u->m_ctl_d.m_end == u->m_ctl_d.m_last)
	{
		return FALSE;
	}	
	/*判断该链表节点才刚刚开始发送
	这个时候可以插入信令回复消息*/
	if(u->m_node->data == u->m_media_d.m_last)
	{
		send_cmd_msg = TRUE;	
	}
	else 
	{
		send_cmd_msg = FALSE;
	}
	return send_cmd_msg;
}


int32_t get_node_data_len(void *data)
{
	int32_t temp = 0;
	msg_header_t *p = (msg_header_t *)data;
	temp = ntohs(p->m_len);
	return temp;
}


void update_user_and_channel(user_t *u)
{
	node_t *n = NULL;
	node_t *head = NULL;
	n = u->m_node->next;
	r_free(u->m_node->data); //释放数据
	head = find_list_head(u->m_channel);
	del_part_list(&head, u->m_node);
	set_list_head(find_chid_index(u->m_channel), n);
	u->m_node = n;
}


int32_t local_send_process(user_t *u)
{
	return 0;
}


int32_t local_recv_process(user_t *u)
{
	
	//printf("[%s] enter !!!\n", __func__);
	if(NULL == u) {
		printf("[local_recv_process] param is NULL!!!\n");
	}
	char buf[CI_LEN] = {0};
	char *recv_buf = buf;
	channel_info_t *ci = NULL;
	int32_t total = sizeof(channel_info_t);
	int32_t	ret = -1;
	node_t *head;
	int32_t index = 0;
	int32_t c_in = 0;
	int32_t list_len = 0;
	user_t *handle = NULL;

	while(1) 
	{
		ret = async_recv_data(u->m_fd, recv_buf, total);
		if(ret < 0) {
			if(-1 == ret) {
				close_client(u);
			}
			break;
		}
		ci = (channel_info_t *)recv_buf;
		if(NULL == ci) {
			printf("[local_recv_process] buf is NULL!!!\n ");
			continue;
			}
		c_in = get_local_data_index( ci->m_channel);
		
		if(0 == get_channel_user_total(ci->m_channel)) {
			//printf("[local_recv_process][%d] []free : [%p]\n",  ci->m_channel, (void *)(ci->m_data));
			r_free((void *)(ci->m_data));
			ret = 2;
			continue;
		}
		
		pack_header_msg(ci->m_data, ci->m_msg_type, ci->m_data_len + MSG_HEAD_LEN);
		list_len = get_list_len(c_in);
		head = get_list_head(c_in);
		//printf("[local_recv_process] append_node before c_in : [%d] head : [%p]\n", c_in, head);
		append_node(&head, (void *)(ci->m_data), NULL);
	//	printf("[local_recv_process]  append_node after c_in : [%d] head : [%p]\n", c_in, head);
		set_list_head(c_in, head);
		list_len ++;
		set_list_len(c_in, list_len);
		if(MAX_LIST_LEN < list_len)
		{
			head = get_list_head(find_chid_index(ci->m_channel));
			for(index = 0; index < MAX_USERS; index++)
			{
				if(get_user_node(index)== head) {
					handle = get_user_handle(index);
					close_client(get_user_handle(index));
				}
			}
			r_free(head->data);
			head->data = NULL;
			//printf("[local_recv_process]  del_part_list before head : [%p] head-next : [%p]\n", head, head->next);
			del_head_node(&head);
			//printf("[local_recv_process]  del_part_list after head : [%p]\n", head);
			set_list_head(find_chid_index(ci->m_channel), head);
			list_len --;	
			set_list_len(c_in, list_len);
		}
		
	//	printf("[%d]=[%d]list_len: [%d]\n", ci->m_channel, find_chid_index(ci->m_channel), list_len);
	//	printf("[local_recv_process] index: [%d]head:[%p]data:[%p]\n", c_in, head, ci->m_data);
		set_new_req_user(ci->m_channel);
	}
	//printf("[%s] end !!!\n", __func__);
	return ret;
}

int ready_next_node(user_t *u , int8_t response)
{
	if(response)
	{
		/*定位到最前面*/
		u->m_ctl_d.m_last = u->sbuf;
		u->m_ctl_d.m_last = u->m_ctl_d.m_end;
	}
	else
	{	
		if(NULL != u->m_node->next) {
			u->m_send_status = IS_SENDING;  //正在发送数据
			u->m_media_d.m_last = (int8_t *)u->m_node->next->data;
			u->m_media_d.m_end = u->m_media_d.m_last + \
									get_node_data_len(u->m_node->next->data);/*该节点数据长度*/
			u->m_node = u->m_node->next;
		}else {
			u->m_send_status = NOT_CAN_SEND;  //没有下一个节点
			u->m_media_d.m_last  = NULL;
			u->m_media_d.m_end = NULL;
			return -1;
		}
	}
	return 0;
}

int32_t send_client_video_data(user_t *u)
{
	//printf("[%s] enter !!!\n", __func__);
	int32_t send_total_len 	= 0 ;
	int32_t slen = 0;
	int8_t* buf 				= NULL;
	int8_t response = FALSE;
	int32_t   ret = 0;
	while(1) {
		/*如果有回复数据要发送*/
		/*可以插入数据发送*/
		if(is_response_msg(u))	
		{
			/*要发送的长度*/
			send_total_len = u->m_ctl_d.m_end - u->m_ctl_d.m_last;
			buf = u->m_ctl_d.m_last;
			response = TRUE;
			printf("--------------------send_total_len  =%d\n",send_total_len );
		}
		else
		{
			//printf("--------------------u->m_send_status  =%d\n",u->m_send_status );
			response = FALSE;
			/*没有下一个结点状态*/
			if(NOT_CAN_SEND == u->m_send_status )
			{
				ret = ready_next_node(u,response);				
				if(ret < 0) {
					//printf("------------------no next node!!\n");
					break;
				}
			}				
			send_total_len = u->m_media_d.m_end - u->m_media_d.m_last;
			//printf("send_total_len = %d \n",send_total_len);
			if(0 == send_total_len) {
				goto next_node;
			}
			buf = u->m_media_d.m_last;
			
		}
		//printf("[send_client_video_data] before : [%d]\n", send_total_len);
		slen= send_total_len;
		ret = async_send_data(u->m_fd, (void *)buf, &send_total_len);
		#if SEND_LIVE_DATA_S
		printf("[%s]---local_id [%d]---liver_ip[%s]---send_len[%d]\n",__func__, u->m_channel ,u->m_ip,send_total_len);
		#endif
		if(ret < 0) {
			//printf("[send_client_video_data]-<%d : %d>-errno : %d ,%d\n", u->m_fd, u->m_channel, errno, send_total_len);
			if(-1 == ret) {
				close_client(u);
				break;
			} 
			else {
				/*计算这次事件发送了多少数据*/
				if(response)
				{
					u->m_ctl_d.m_last += (slen - send_total_len);
				}
				else
				{					
					u->m_media_d.m_last += (slen - send_total_len);
				}
				u->m_send_status = NOT_AFTER_SENDING;   //该结点数据未发送完成
				
				break;
			}		
		}
next_node:
		/* 是否准备发送下一个节点*/
		ready_next_node(u , response);		
	}
	//printf("[%s] end !!!\n", __func__);
	return ret;
}

int32_t net_send_process(user_t *u)
{
	//printf("[net_send_process] @@@@@@@@@@@@[%d]:[%d]\n",  u->m_fd, u->m_is_send);
	if(1 == u->m_is_send) {
		send_client_video_data(u);
	} else {
		return -1;
	}
	return 0;
}

static int32_t net_msg_reap_send(int8_t *sp, int32_t fd, int32_t msg, void *buf, int32_t buflen)
{
	int32_t length = 0;
	int32_t ret = -1;
	length += MSG_HEAD_LEN;
	r_memcpy((void *)(sp + MSG_HEAD_LEN), buf, buflen);
	length +=sizeof(int32_t);
	pack_header_msg((void *)sp, msg, length);				
	ret = async_send_data(fd, (void *)sp, (int32_t *)&length);
	return ret;
}

/*接收消息数据，返回是否需要响应数据给客户端*/
#if 0
int32_t net_recv_process(user_t *u)
{
	//printf("[net_recv_process] enter!!!\n");
	int32_t remain = 0;
	int32_t ret = 0;
	int32_t chid = 0;
	int32_t user_login = USER_LOGIN_ERROR;
	int32_t online_user = 0;
	int8_t *rp = u->rbuf;
	int8_t *sp = u->sbuf;
	while(1) {
		//printf("----------1----------\n");
		ret = async_recv_data(u->m_fd, rp, MSG_HEAD_LEN);
		if(ret < 0) {
			printf("[net_recv_process]-<1>-errno : %d\n", errno);
			if(-1 == ret) {
				close_client(u);
			}
			break;
		}
		
		msg_header_t *p = (msg_header_t *)rp;
		remain = ntohs(p->m_len) - MSG_HEAD_LEN ;
		//printf("[net_recv_process] total : %d [%d]\n", remain, p->m_msg_type);
		if(0 == remain) {
			break;
		}
		int8_t *rdata = rp + MSG_HEAD_LEN;
		ret = async_recv_data(u->m_fd, rdata, remain);
		if(ret < 0) {
			printf("[net_recv_process]-<2>-errno : %d\n", errno);
			perror("[net_recv_process] recv");
			if(-1 == ret) {
				close_client(u);
			}
			break;
		}
	//	printf("[net_recv_process] m_msg_type : [%d]\n", p->m_msg_type);
		switch(p->m_msg_type) {
		case	MSG_TYPE_LOGIN:
			online_user = get_online_user_total();
			printf("MSG_TYPE_LOGIN : [%d]\n", online_user);
			if(PERM_ONLINE_USERS  <= online_user) {
				user_login = USER_LOGIN_MAXUSERS;
				ret = net_msg_reap_send(sp, u->m_fd, MSG_TYPE_LOGIN, (void*)&user_login, sizeof(int32_t));
				if(ret < 0) {
					if(-1 == ret) {
						close_client(u);
						return -1;
					}
					
				}
				u->m_is_send = -1;
				
				break;
			}
			user_login = check_net_user((net_user_info_t *)rdata);
			printf("user_login is  -- [%d]\n",user_login);
			if(user_login) {
				ret = net_msg_reap_send(sp, u->m_fd, MSG_TYPE_LOGIN, (void*)&user_login, sizeof(int32_t));
				if(ret < 0) {
					if(-1 == ret) {
						close_client(u);
						return -1;
					}
					
				}
				u->m_is_send = 0;
			} else {
				ret = net_msg_reap_send(sp, u->m_fd, MSG_TYPE_LOGIN, (void*)&user_login, sizeof(int32_t));
				if(ret < 0) {
					if(-1 == ret) {
						close_client(u);
						return -1;
					}
					
				} 
				u->m_is_send = -1;
			}
			break;
		case	MSG_TYPE_DATAREQ:
			chid = *((int32_t*)(rp + MSG_HEAD_LEN));
			printf("[net_recv_process] chid : [%d]\n", chid);
			if(is_valid_channel_id(chid) && 0 == u->m_is_send) {
				
				u->m_channel = chid;
				ret = net_msg_reap_send(sp, u->m_fd, MSG_TYPE_FIREWALL, (void*)&chid, sizeof(int32_t));
				printf("net_msg_reap_send is -----\n ");

				if(ret < 0) {
					if(-1 == ret) {
						printf("-1 == ret_1\n");
						close_client(u);
						return -1;
					}
					
				}
			} else {
				chid = get_valid_channel_id();
				ret = net_msg_reap_send(sp, u->m_fd, MSG_TYPE_FIREWALL, (void*)&chid, sizeof(int32_t));
				if(ret < 0) {
					if(-1 == ret) {
						printf("-1 == ret_2\n");
						close_client(u);
						return -1;
					}
					
				}
				u->m_is_send = -1;
			}
			break;
		default:
			printf("[net_recv_process] default\n");
			user_login = USER_LOGIN_ERROR;
			ret = net_msg_reap_send(sp, u->m_fd, MSG_TYPE_LOGIN, (void*)&user_login, sizeof(int32_t));
			if(ret < 0) {
				if(-1 == ret) {
					printf("-1 == ret_3\n");
					close_client(u);
					return -1;
				}
				
			}
			u->m_is_send = -1;
			break;
		}
		if(-1 == u->m_is_send &&  -EAGAIN != ret) {
			printf("-1 == ret_4\n");
			close_client(u);
			return ret;
		}
	}
	printf("[%s] end !!!\n", __func__);
	return ret;
}
#endif
int32_t net_recv_process(user_t *u)
{
	//printf("[net_recv_process] enter!!!\n");
	int32_t remain = 0;
	int32_t ret = 0;
	int32_t chid = 0;
	int32_t user_login = USER_LOGIN_ERROR;
	int32_t online_user = 0;
	int32_t msg = -1;
	void *sbuf = NULL;
	int32_t sbuf_len = 0;
	int8_t *rp = u->rbuf;
	int8_t *sp = u->sbuf;

	while(1) {
		//printf("----------1----------\n");
		ret = async_recv_data(u->m_fd, rp, MSG_HEAD_LEN);

		if(ret < 0) {
			printf("[net_recv_process]-<1>-errno : %d\n", errno);

			if(-1 == ret) {
				close_client(u);
			}

			break;
		}

		msg_header_t *p = (msg_header_t *)rp;
		remain = ntohs(p->m_len) - MSG_HEAD_LEN ;

		//printf("[net_recv_process] total : %d [%d]\n", remain, p->m_msg_type);
		if(0 == remain) {
			break;
		}

		int8_t *rdata = rp + MSG_HEAD_LEN;
		ret = async_recv_data(u->m_fd, rdata, remain);

		if(ret < 0) {
			printf("[net_recv_process]-<2>-errno : %d\n", errno);
			perror("[net_recv_process] recv");

			if(-1 == ret) {
				close_client(u);
			}

			break;
		}

		//	printf("[net_recv_process] m_msg_type : [%d]\n", p->m_msg_type);
		switch(p->m_msg_type) {
			case	MSG_TYPE_LOGIN:
				online_user = get_online_user_total();
				printf("MSG_TYPE_LOGIN : [%d]\n", online_user);

				if(PERM_ONLINE_USERS  <= online_user) {
					user_login = USER_LOGIN_MAXUSERS;
					u->m_is_send = -1;
					break;
				}

				user_login = check_net_user((net_user_info_t *)rdata);
				printf("user_login is  -- [%d]\n", user_login);

				if(user_login) {
					u->m_is_send = 0;
				} else {
					u->m_is_send = -1;
				}

				msg = MSG_TYPE_LOGIN;
				sbuf = (void *)&user_login;
				sbuf_len = sizeof(int32_t);
				break;

			case	MSG_TYPE_DATAREQ:
				chid = *((int32_t *)(rp + MSG_HEAD_LEN));
				printf("[net_recv_process] chid : [%d]\n", chid);

				if(is_valid_channel_id(chid) && 0 == u->m_is_send) {
					u->m_channel = chid;
				} else {
					chid = get_valid_channel_id();
					u->m_is_send = -1;
				}

				msg = MSG_TYPE_FIREWALL;
				sbuf = (void *)&chid;
				sbuf_len = sizeof(int32_t);
				break;

			default:
				printf("[net_recv_process] default\n");
				user_login = USER_LOGIN_ERROR;
				msg = MSG_TYPE_LOGIN;
				sbuf = (void *)&user_login;
				sbuf_len = sizeof(int32_t);
				u->m_is_send = -1;
				break;
		}

		ret = net_msg_reap_send(sp, u->m_fd, msg, sbuf, sbuf_len);

		if(-1 == u->m_is_send || -1 == ret) {
			close_client(u);
			return ret;
		}
	}

//	printf("[%s] end !!!\n", __func__);
	return ret;
}


int32_t send_user_process(void)
{
	//printf("[%s] enter !!!\n", __func__);
	int index = 0;
	user_t *u = NULL;
	for(index = 0; index < MAX_USERS; index++)
	{	
		u = get_user_handle(index);
		if(u->m_invalid && u->send_process) {
			u->send_process(u);
		}
	}
	//printf("[%s] end !!!\n", __func__);
	return 0;
}





