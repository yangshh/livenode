#ifndef	__USER_H__
#define	__USER_H__


#include "stdint.h"
#include "reach_list.h"
#include "reach_epoll.h"
#include "reach_socket.h"
#include "key.h"
#define	MAX_RECV_LEN	265
#define	SEND_MSG_LEN		2048

typedef  struct asyn_sends
{
	int8_t *  m_last;   			/*have sended data  end pointer*/
	int8_t *  m_end;	  			/*need to send data end pointer*/
	uint32_t  m_fail;		/*failed times*/
}asyn_send;

typedef struct users user_t;
struct users {
	int32_t m_invalid;   //��sock�Ƿ���Ч
	int32_t m_is_send;  //�Ƿ��Ȩͨ��
	int32_t m_send_status; //���͵�״̬
	//lock_t  m_lock;
	int32_t m_channel;//�û�����ͨ��id
	int32_t m_fd;
	int8_t m_ip[IP_LEN];
	/*epoll�¼�*/
	int32_t m_epfd;
	ev_t *m_ev;
	int32_t m_events;
	/*���涨�巢���������*/
	node_t *m_node;   //��������
	asyn_send m_media_d;   //ý�����ݷ���
	asyn_send m_ctl_d;		//������Ϣ�ظ�����
	
	/*��Ա����*/
	int32_t (*recv_process)(user_t *);
	int32_t (*send_process)(user_t *);
	int8_t sbuf[SEND_MSG_LEN];   //����ظ�������buf
	int8_t rbuf[MAX_RECV_LEN];
};
user_t  g_send_client[MAX_USERS];

int32_t get_live_user_info(void **buf);

user_t *get_user_handle(int32_t index);
user_t* find_user_handle(int32_t fd);
int32_t get_new_user_index(void);
int32_t get_valid_user_total(void);
int32_t get_online_user_total(void);

node_t *get_user_node(int32_t index);

void init_client_info(user_t *u);
void init_client_info_table(void);
void add_local_client_info_table(int32_t epfd, int32_t fd, ev_t *ev, int32_t events);
void add_net_client_info_table(int32_t epfd, int32_t fd, ev_t *ev, int32_t events, char *ip);
void close_client(user_t *u);
int32_t del_live_user(int8_t *ip, int32_t chid);
int32_t get_channel_user_total(uint16_t chid);
int32_t send_user_process(void);
int32_t set_new_req_user(uint16_t chid);
#endif	//__USER_H__

