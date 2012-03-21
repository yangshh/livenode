#ifndef __FILE_WEB_PROTOCOL_RESOLVE_H
#define __FILE_WEB_PROTOCOL_RESOLVE_H

#define WEB_G_IP_MSG				"/tmp/web_g_ip_msg.txt"

#include "connect_rec_server.h"
#include "xml_msg_management.h"
#include<semaphore.h>



#define	ONE_REPORT_MSG_LEN		(256)	//�ϱ���Ϣ��ÿ��ͨ���ĳ���
#define		WEB_MSG_LEN			(ONE_REPORT_MSG_LEN * CHANNEL_CONNECT_MAX_NUM)	//web������Ϣ�ĳ���
#define 	WEB_TEMP_MSG_LEN		10

// http MsgCode
#define    MSGCODE_HTTP_REC_SERV              10001
#define    MSGCODE_HTTP_LIVE_USERS           10002
#define    MSGCODE_HTTP_LIVE_CHANNEL       10003
// tcp MsgCode
#define    MSGCODE_TCP_REC_SERV                20001
#define    MSGCODE_TCP_LIVE_CHANNEL         20002
#define    MSGCODE_TCP_LIVE_USERS             20003
#define    MSGCODE_TCP_SYS_INFO                 20004
#define    MSGCODE_TCP_STOP_LIVE               20005
#define    MSGCODE_TCP_REBOOT_PC             20006

typedef struct 	web_client_infos
{
	int32_t	m_client_socket;	//�����ϵĿͻ��˵�socket
}web_client_info_t;

typedef struct      web_client_infos_ex
{
	int32_t   m_client_socket;
	struct sockaddr_in m_client_addr;
} web_client_info_ex_t;

typedef struct	web_global_vaiables
{
	sem_t m_sem;	//�����߳�ʱ���ź���,���ڵȴ�һ���ͻ��˴����ɹ����ٴ����ڶ���
}web_global_vaiables_t;

// zhengyb add 12_2_2

/**
* @brief ͨ��ȫ�ֱ�����ȡý�����ĵ�ַ
* @param  [OUT] ip ý�����ĵ�ַ
* @param  [IN] NULL
* @exception  -1
* @return �ɹ�1 ʧ�ܷ���0
*/
int32_t get_web_url(char *ip);

/**
* @brief ��ȡ��������(ETH_NAME �궨������)��IP Ϊֱ�����IP
* @param  [OUT]ֱ�����IP
* @param  [IN] NULL
* @exception  -1
* @return �ɹ�0 ʧ�ܷ���-1
*/
int32_t fine_local_ip(char *ip);

/**
* @brief ֱ�����ѭ���ϱ�ִ�к���
* @param  [OUT] NULL
* @param  [IN] NULL
* @exception  -1
* @return NULL
*/
void judge_user_channel_http_report();




/**
* @brief ����ȫ�ֱ���g_connect_info Ѱ���������Ԫ��
* @param  [OUT] g_connect_info �����±�
* @param  [IN] NULL
* @exception  -1
* @return �ɹ����������±�ֵ
*/
int32_t Traverse_gAmarry_free();

/**
* @brief ����ȫ�ֱ���g_connect_info Ѱ��������״̬Ԫ��
* @param  [OUT] g_connect_info �����±�
* @param  [IN] NULL
* @exception  -1
* @return �ɹ����������±�ֵ
*/
int32_t Traverse_gAmarry_usd();

/**
* @brief ����ֱ�������WEB ��msgcode_tcp_rec_serv TCP Э��
* @param  [OUT] p_buffer ��װ����ϱ���Ϣ
* @param  [IN] ����WEB�·���TCP ������Ϣ
* @exception  -1
* @return �ɹ�����0
*/
int32_t msgcode_tcp_rec_serv_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/**
* @brief ����ֱ�������WEB ��msgcode_tcp_live_channel TCP Э��
* @param  [OUT] p_buffer ��װ����ϱ���Ϣ
* @param  [IN] ����WEB�·���TCP ������Ϣ
* @exception  -1
* @return �ɹ�����0
*/
int32_t msgcode_tcp_live_channel_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/**
* @brief ����ֱ�������WEB ��msgcode_tcp_live_users TCP Э��
* @param  [OUT] p_buffer ��װ����ϱ���Ϣ
* @param  [IN] ����WEB�·���TCP ������Ϣ
* @exception  -1
* @return �ɹ�����0
*/
int32_t msgcode_tcp_live_users_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/**
* @brief ����ֱ�������WEB �� msgcode_tcp_sys_info TCP Э��
* @param  [OUT] p_buffer ��װ����ϱ���Ϣ
* @param  [IN] ����WEB�·���TCP ������Ϣ
* @exception  -1
* @return �ɹ�����0
*/
int32_t msgcode_tcp_sys_info_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/**
* @brief  ��װhttp post �����ȡ¼����ϢΪxml ģʽ
* msgcode  -> MSGCODE_HTTP_REC_SERV
* @param  [OUT] ���xml ģʽ¼����Ϣ��buffer 
* @param  [IN] int8_t *
* @exception  -1
* @return �ɹ�����0
*/
int32_t package_web_cmd_http_ServInfoReq(int8_t *buffer);

/**
* @brief  ��װhttp post �ϱ�ֱ�������û���ϢΪxml ģʽ
* msgcode  -> MSGCODE_HTTP_LIVE_USERS
* @param  [OUT] ���xml ģʽ�����û���Ϣ��buffer 
* @param  [IN] int8_t *
* @exception  -1
* @return �ɹ�����0
*/
int32_t package_web_cmd_http_LiveUpdateReq(int8_t *buffer);

/**
* @brief  ��װhttp post �ϱ�¼��������ͨ����ϢΪxml ģʽ
* msgcode  -> MSGCODE_HTTP_LIVE_CHANNEL
* @param  [OUT] ���xml ģʽ¼��������ͨ����Ϣ��buffer 
* @param  [IN] int8_t *
* @exception  -1
* @return �ɹ�����0
*/
int32_t package_web_cmd_http_LiverChannelUpdateReq(int8_t *buffer);

/**
* @brief  ����http post Э��1  ������Ϣ������¼�������� 
* msgcode  -> MSGCODE_HTTP_REC_SERV
* @param  [OUT] NULL
* @param  [IN] ���xml ģʽ¼����Ϣ��buffer 
* @exception  -1
* @return �ɹ�����0
*/
int32_t parse_web_cmd_http_ServInfoReq(int8_t *buffer);

/**
* @brief  ����http post ������Ϣ���ж��ϱ��û���ͨ����Ϣ�Ƿ�ý�������������� 
* msgcode  -> MSGCODE_HTTP_LIVE_USERS and MSGCODE_HTTP_LIVE_CHANNEL
* @param  [OUT] NULL
* @param  [IN] ���xml ģʽpost �ϱ��󷵻���Ϣ��buffer 
* @exception  -1
* @return �ɹ�����0
*/
int32_t parse_ResponseMsg(int8_t *buffer);

/**
* @brief  ����ý�������·�xmlģʽTCP ������Ϣ���Ϊreq_msg_serv_channel_t�ṹ������
* msgcode  -> ����TCP ����ģʽ
* @param  [OUT] ���ý�������·�TCP ������Ϣ��req_msg_serv_channel_t
* @param  [OUT]    req_msg_code ý�������·�TCP ����������
* @param  [IN] ���ý�������·�xmlģʽTCP  ������Ϣ
* @exception  -1
* @return �ɹ�����0
*/
int32_t	parse_web_cmd_tcp(req_msg_serv_channel_t* req_msg_room_id,int32_t* req_msg_code, int8_t 
*buffer);

/**
* @brief ��ʱ��httpģʽ�ϱ�ֱ�������û���Ϣ
* msgcode  -> MSGCODE_HTTP_LIVE_USERS
* @param  [OUT] NULL
* @param  [IN] ý��ý������IP ��ַ
* @exception  -1
* @return �ɹ�����0
*/
int32_t web_post_live_user_info(int8_t *post_url);

/**
* @brief  ��ʱ��httpģʽ�ϱ�¼��������ͨ����Ϣ
* msgcode  -> MSGCODE_HTTP_LIVE_CHANNEL
* @param  [OUT] NULL
* @param  [IN] ý��ý������IP ��ַ
* @exception  -1
* @return �ɹ�����0
*/
int32_t web_post_channel_update_info(int8_t *post_url);

/**
* @brief  ������ý������TCP ������̺߳���
* @param  [OUT] NULL
* @param  [IN] �����ϵĿͻ��˵�socket
* @exception  NULL
* @return  NULL
*/
//static void* create_listen_client_thread(void* arg);

/**
* @brief  ������ý������http ������̺߳���
* �˺����ж�TCP �����������������Ӧ�Ĵ���!!!!!!
* @param  [OUT] NULL
* @param  [IN] NULL
* @exception  NULL
* @return  NULL
*/
//static void* create_http_report_thread(void* arg);

/**
* @brief  ������ý������tcp ���ӵ��̺߳���
* ����������TCP �����߳�create_listen_client_thread 
* @param  [OUT] NULL
* @param  [IN] NULL
* @exception  NULL
* @return  NULL
*/
//static void* create_web_listen_thread(void* arg);

int32_t	init_web_protoclo_resolve_func(void);
#endif

