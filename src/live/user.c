#include "stdint.h"
#include <stdio.h>
#include "user.h"
#include "client.h"
#include "reach_os.h"
#include "media_msg.h"
#include "reach_list.h"
#include "local_data.h"
#include "reach_epoll.h"
/*client infomation handle*/
#define	UI_LEN	22
int8_t live_user_info[MAX_USERS * UI_LEN] = {0};
user_t *get_user_handle(int32_t index)
{
	if(0 > index || MAX_USERS < index)
		return NULL;
	return &(g_send_client[index]);
}



node_t *get_user_node(int32_t index)
{
	user_t *u = get_user_handle(index);
	if(NULL == u) {
		return NULL;
	}
	return u->m_node;
}

user_t* find_user_handle(int32_t fd)
{
	int index = 0;
	user_t *u = NULL;
	for(index = 0; index < MAX_USERS; index++)
	{	
		u = get_user_handle(index);
		if(u->m_fd == fd)
		{
			return u;
		}
	}
	/*异常情况处理*/
	return NULL;
}

int32_t set_new_user_send_date(user_t *u, uint16_t chid)
{
	if(NULL == u) {
		return -1;
	}
	u->m_node = find_list_end(chid);
	if(NULL == u->m_node) {
		printf("[set_new_user_send_date] NULL == u->m_node\n");
		return -1;
	}
	u->m_media_d.m_last = (int8_t *)(u->m_node->data);
	u->m_media_d.m_end = (int8_t *)((u->m_node->data) + get_node_data_len(u->m_node->data));
	u->m_media_d.m_fail = 0;
	u->m_send_status = NOT_AFTER_SENDING;
	u->m_is_send = 1;
	return 0;
}

int32_t set_new_req_user(uint16_t chid)
{
	int32_t index = 0;
	int32_t ret = -1;
	user_t *u = NULL;
	for(index = 0; index < MAX_USERS; index++)
	{	
		u = get_user_handle(index);
		if(NULL == u) {
			return ret;
		}
		if((NULL == u->m_node) && (NOT_SENT== u->m_send_status)) 
		{
			if(u->m_channel == chid)
			{
				ret = set_new_user_send_date(u, chid);
			}
		}
	}
	return ret;
}

int32_t get_new_user_index(void)
{
	int index = 0;
	user_t *u = NULL;
	for(index = 0; index < MAX_USERS; index++)
	{
		u = get_user_handle(index);
		if(u->m_invalid == 0)
		{
			return index;
		}
	}
	return -1;
}

int32_t get_valid_user_total(void)
{
	int index = 0;
	int total = 0;
	user_t *u = NULL;
	for(index = 0; index < MAX_USERS; index++)
	{
		u = get_user_handle(index);
		if(u->m_invalid == 1)
		{
			return total ++;
		}
	}
	return 0;
}



void init_client_info(user_t *u)
{
	if(NULL == u) {
		printf("[init_client_info] param is NULL!!\n");
		return;
	}
	u->m_invalid = 0;
	u->m_fd = -1;
	r_memset(u->m_ip, 0, IP_LEN);
	u->m_epfd = -1;
	u->m_ev = NULL;
	u->m_events = -1;
	u->m_node = NULL;
	r_memset(&(u->m_media_d), 0, sizeof(asyn_send));
	r_memset(&(u->m_ctl_d), 0, sizeof(asyn_send));
	u->recv_process = NULL;
	u->send_process = NULL;
	r_memset(u->sbuf, 0, SEND_MSG_LEN);
	r_memset(u->rbuf, 0, MAX_RECV_LEN);
	u->m_ctl_d.m_last = u->sbuf;
	u->m_ctl_d.m_end = u->m_ctl_d.m_last;
	u->m_ctl_d.m_fail = 0;
	u->m_send_status = NOT_SENT;
	u->m_is_send = -1;
	u->m_channel = -1;
}

void init_client_info_table(void)
{
	int32_t index = 0;
	user_t *u = NULL;
	for(index = 0; index < MAX_USERS; index++)
	{
		u = get_user_handle(index);
		init_client_info(u);
	}
}

void add_local_client_info_table(int32_t epfd, int32_t fd, ev_t *ev, int32_t events)
{
	user_t *u = NULL;
	/*找到一个新位置*/
	int index = get_new_user_index();
	if(index < 0)
	{
		return;
	}
	u = get_user_handle(index);
	u->m_fd = fd;
	u->m_epfd = epfd;
	u->m_ev = ev;
	u->m_events = events;
	u->m_invalid = 1;
	u->recv_process = local_recv_process;
	u->send_process = local_send_process;
	add_epoll_event(epfd, fd, ev, events);
}
	
void add_net_client_info_table(int32_t epfd, int32_t fd, ev_t *ev, int32_t events, char *ip)
{
	user_t *u = NULL;
	/*找到一个新位置*/
	int index = get_new_user_index();
	if(index < 0)
	{
		printf("[add_net_client_info_table] new err!!\n");
		return;
	}
	u = get_user_handle(index);
	u->m_fd = fd;
	r_memcpy(u->m_ip, ip, IP_LEN);
	u->m_epfd = epfd;
	u->m_ev = ev;
	u->m_events = events;
	u->m_invalid = 1;
	u->recv_process = net_recv_process;
	u->send_process = net_send_process;
	printf("[add_net_client_info_table] m_ev : [%p]\n", u->m_ev);
	add_epoll_event(epfd, fd, ev, events);
}

void close_client(user_t *u)
{
	static int32_t n = 0;
	if(NULL != u->m_ev) {
		del_epoll_event(u->m_epfd, u->m_fd, u->m_ev, u->m_events);
	} else {
		printf("[close_client] ev is NULL!!!\n");
	}
	printf("[close_client] =============@@@--<%d>--@@@================= fd : [%d] ip : [%s] ch : [%d]\n", n ++, u->m_fd, u->m_ip, u->m_channel);
	close_socket(u->m_fd);
	init_client_info(u);
}

int32_t del_live_user(int8_t *ip, int32_t chid)
{
	int32_t  index = 0;
	user_t *u = NULL;
	int32_t user_num = 0;
	printf("ip is  --- [%s]\n",ip);
	printf("chid is --- [%d]\n",chid);
	for(index = 0; index < MAX_USERS; index++)
	{      
		u = get_user_handle(index);
		if(u->m_channel != -1)
		{
			if(u->m_channel == chid && 0 == r_strcmp((const int8_t *)ip, (const int8_t *)u->m_ip)) 
			{
				printf("[del_live_user] ------------------ del user ok !!\n");
				close_client(u);
				user_num++;
			}
		}		
		
	}
	if(user_num !=0)
	{
		return 1;
	}
	return 0;
}

int32_t get_channel_user_total(uint16_t chid)
{
	int32_t  index = 0;
	int32_t  total = 0;
	user_t *u = NULL;
	for(index = 0; index < MAX_USERS; index++)
	{
		u = get_user_handle(index);
		if(u->m_channel == chid) {
			++total;
			//printf("[get_channel_user_total][%d] id : [%d] [%d]\n", u->m_fd, chid, total);
		}
	}
	return total;
}

int32_t get_online_user_total(void)
{
	int32_t  index = 0;
	int32_t  total = 0;
	user_t *u = NULL;
	for(index = 0; index < MAX_USERS; index++)
	{
		u = get_user_handle(index);
		if(u->m_channel != -1 ) {
			++total;
	//		printf("[get_online_user_total][%d] id : [%d] [%d]\n", u->m_fd, u->m_channel, total);
		}
	}
//	printf("total is %d\n",total);
	return total;
}

int32_t Ruled_out_zero(char *dest,char *src,int src_len)
{
	int i_loop = 0;
	int dest_len = 0;
	if(src == NULL)
	{
		return -1;
	}
	for(; i_loop < src_len;i_loop++)
	{
		if( (src[i_loop] > '-') && ( src[i_loop]  < ';'))
		{
			dest[dest_len] = src[i_loop];
			dest_len++;
		}
	}
	return dest_len;
}

int32_t get_live_user_info(void **buf)
{	
	int32_t ut = get_online_user_total();
	if(ut == 0) {
		memset(live_user_info, 0, (MAX_USERS * UI_LEN));
		return -2;
	}
	int8_t *lui = r_malloc(ut * UI_LEN);
	if(NULL == lui) {
		printf("NULL == lui\n");
		return -1;
	}
	memset(lui,0,ut * UI_LEN);
	int8_t *temp_lui = r_malloc(ut * UI_LEN);
	if( NULL == temp_lui)
	{
		printf("NULL == temp_lui\n");
		return -1;
	}
	memset(temp_lui,0,ut * UI_LEN);
	int32_t  index = 0;
	user_t *u = NULL;
	int32_t tlen = 0;
	for(index = 0; index < MAX_USERS; index++)
	{
		u = get_user_handle(index);
		if(u->m_channel != -1) {
//			printf("u->m_ip is ---[%s]\n",u->m_ip);
			r_snprintf((lui + tlen), UI_LEN, "%s:%d/", u->m_ip, u->m_channel);
//			printf("lui is ---[%s]\n",&lui[tlen]);
			tlen += UI_LEN;
		}
	}
         int32_t temp_lui_len = Ruled_out_zero(temp_lui,lui,tlen);
	temp_lui[temp_lui_len-1] = '\0';
//	printf("temp_lui is --- [%s] temp_lui_len is ---[%d]\n",temp_lui,temp_lui_len);
	printf("[get_live_user_info] ++++++++++++++++++++++++++++ noline : [%d]\n", ut);
	if(temp_lui_len > 0 )
	{
		int32_t strcmp_code = r_strcmp((const int8_t *)live_user_info, (const int8_t *)temp_lui);
		if(strcmp_code == 0)
		{
			printf("there is no change of the live users!\n");		
			*buf = live_user_info;
			r_free(temp_lui);
			r_free(lui);
			return 0;
		}
		else
		{
			memcpy(live_user_info, temp_lui,temp_lui_len);
			*buf = live_user_info;
			r_free(temp_lui);
			r_free(lui);		
			return ut;
		}		
	}	
	r_free(temp_lui);
	r_free(lui);
	return -1;
}

