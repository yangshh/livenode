#ifndef __FILE_WEB_PROTOCOL_RESOLVE_H
#define __FILE_WEB_PROTOCOL_RESOLVE_H

#define WEB_G_IP_MSG						"/tmp/web_g_ip_msg.txt"

#include "xml_msg_management.h"
#include <semaphore.h>

#define	ONE_REPORT_MSG_LEN				(256)	//�ϱ���Ϣ��ÿ��ͨ���ĳ���
#define		WEB_MSG_LEN				(ONE_REPORT_MSG_LEN * CHANNEL_CONNECT_MAX_NUM)	//web������Ϣ�ĳ���
#define 	WEB_TEMP_MSG_LEN				  10

// ֱ�������ý�����Ľ���HTTP Э�� MsgCode
#define    MSGCODE_HTTP_REC_SERV                     10001
#define    MSGCODE_HTTP_LIVE_USERS                  10002
#define    MSGCODE_HTTP_LIVE_CHANNEL       	  10003
// ֱ�������ý�����Ľ���TCP Э�� MsgCode
#define    MSGCODE_TCP_REC_SERV              	  20001
#define    MSGCODE_TCP_LIVE_CHANNEL        	  20002
#define    MSGCODE_TCP_LIVE_USERS             	  20003
#define    MSGCODE_TCP_SYS_INFO                 	  20004
#define    MSGCODE_TCP_STOP_LIVE               	  20005
#define    MSGCODE_TCP_REBOOT_PC             	  20006

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

/*
****************************************************************************
**��������: get_web_url()
**��������: ͨ��ȫ�ֱ���g_web_ip ��ȡý�����ĵ�ַ
**�������: NULL	
**�������:  ip ---ý�����ĵ�ַ
**����ֵ       :  �ɹ�����0 	ʧ�ܷ���-1
****************************************************************************
*/
int32_t get_web_url(char *ip);

/*
****************************************************************************
**��������: set_wet_url()
**��������: ÿ��ý����������ֱ�������Ҫ��ý�����ĵ�ַ�����б���
**�������:  ip---ý�����ĵ�ַ	
**�������:  NULL
**����ֵ       :  �ɹ�����0 	ʧ�ܷ���-1
****************************************************************************
*/
int32_t set_wet_url(char *ip,void *vag);

/*
****************************************************************************
**��������: judge_user_channel_http_report()
**��������: ֱ�����ѭ���ϱ�ִ�к���
**�������: 	NULL
**�������:    NULL
**����ֵ      :    NULL
****************************************************************************
*/
void judge_user_channel_http_report();

/*
****************************************************************************
**��������: Traverse_gAmarry_free()
**��������: ����ȫ�ֱ���g_connect_info Ѱ���������Ԫ��
**�������: 	NULL
**�������:    NULL
**����ֵ       :   �ɹ����������±�ֵ 	ʧ�ܷ���-1
****************************************************************************
*/
int32_t Traverse_gAmarry_free();

/*
****************************************************************************
**��������: Traverse_gAmarry_usd()
**��������: ����ȫ�ֱ���g_connect_info Ѱ��������״̬Ԫ��
**�������: 	NULL
**�������:    NULL
**����ֵ      :   �ɹ����������±�ֵ 	ʧ�ܷ���-1
****************************************************************************
*/
int32_t Traverse_gAmarry_usd();

/*
****************************************************************************
**��������: msgcode_tcp_rec_serv_func()
**��������: ����ֱ�������WEB ��msgcode_tcp_rec_serv TCP Э��
**�������: 	req_msg_room_id ---����WEB�·���TCP ������Ϣ
**�������:   p_buffer ---��װ����ϱ���Ϣ
**����ֵ       :  �ɹ�����0 	ʧ�ܷ���-1
****************************************************************************
*/
int32_t msgcode_tcp_rec_serv_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/*
****************************************************************************
**��������: msgcode_tcp_live_channel_func()
**��������: ����ֱ�������WEB ��msgcode_tcp_rec_serv TCP Э��
**�������: 	req_msg_room_id ---����WEB�·���TCP ������Ϣ
**�������:   p_buffer ---��װ����ϱ���Ϣ
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t msgcode_tcp_live_channel_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/*
****************************************************************************
**��������: msgcode_tcp_live_users_func()
**��������: ����ֱ�������WEB ��msgcode_tcp_live_users TCP Э���
**�������: 	req_msg_room_id ---����WEB�·���TCP ������Ϣ
**�������:   p_buffer ---��װ����ϱ���Ϣ
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t msgcode_tcp_live_users_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/*
****************************************************************************
**��������: msgcode_tcp_sys_info_func()
**��������:  ����ֱ�������WEB �� msgcode_tcp_sys_info TCP Э��
**�������: 	req_msg_room_id ---����WEB�·���TCP ������Ϣ
**�������:   p_buffer ---��װ����ϱ���Ϣ
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t msgcode_tcp_sys_info_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/*
****************************************************************************
**��������: package_web_cmd_http_ServInfoReq()
**��������:  ��װ��ȡ¼����Ϣ��HTTP �ϱ�����Ϊxml ģʽ
**�������:  NULL
**�������:   p_buffer ---��װ����ϱ���Ϣ
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t package_web_cmd_http_ServInfoReq(int8_t *buffer);

/*
****************************************************************************
**��������: package_web_cmd_http_LiveUpdateReq()
**��������:  ��װֱ����������û���ϢΪHTTP�ϱ���Ϣ (xml ģʽ)
**�������:  NULL
**�������:   p_buffer ---��װ����ϱ���Ϣ
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t package_web_cmd_http_LiveUpdateReq(int8_t *buffer);

/*
****************************************************************************
**��������: package_web_cmd_http_LiverChannelUpdateReq()
**��������:  ��װֱ�����ͨ����ϢΪHTTP�ϱ���Ϣ (xml ģʽ)
**�������:  NULL
**�������:   p_buffer ---��װ����ϱ���Ϣ
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t package_web_cmd_http_LiverChannelUpdateReq(int8_t *buffer);

/*
****************************************************************************
**��������: parse_web_cmd_http_ServInfoReq()
**��������:  ������ȡ¼����ϢHTTP Э�鷵����Ϣ������¼�������� 
**�������:  ���xml ģʽpost �ϱ��󷵻���Ϣ��buffer 
**�������:   NULL
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t parse_web_cmd_http_ServInfoReq(int8_t *buffer);

/*
****************************************************************************
**��������: parse_ResponseMsg()
**��������:  ����HTTP Э�鷵�����ж�ֱ�������ý������ͨѶ�Ƿ�����
			       msgcode  -> MSGCODE_HTTP_LIVE_USERS and MSGCODE_HTTP_LIVE_CHANNEL
**�������:  ���xml ģʽpost �ϱ��󷵻���Ϣ��buffer 
**�������:   NULL
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t parse_ResponseMsg(int8_t *buffer);

/*
****************************************************************************
**��������:  parse_web_cmd_tcp()
**��������:  ����ý�������·�xmlģʽTCP ������Ϣ���Ϊreq_msg_serv_channel_t�ṹ������
			       msgcode  -> ����TCP ����ģʽ
**�������:  buffer ---���ý�������·�xmlģʽTCP  ������Ϣ
**�������:  req_msg_code  ---ý�������·�TCP ����������
			       req_msg_room_id ---���ý�������·�TCP ������Ϣ
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t	parse_web_cmd_tcp(req_msg_serv_channel_t* req_msg_room_id,int32_t* req_msg_code, int8_t 
*buffer);

/*
****************************************************************************
**��������: web_post_live_user_info()
**��������:  ��ʱ��httpģʽ�ϱ�ֱ�������û���Ϣ
**�������:  post_url ---ý��ý������IP ��ַ
**�������:   NULL
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t web_post_live_user_info(int8_t *post_url);

/*
****************************************************************************
**��������: web_post_channel_update_info()
**��������:  ��ʱ��httpģʽ�ϱ�¼��������ͨ����Ϣ
**�������:  post_url ---ý��ý������IP ��ַ
**�������:   NULL
**����ֵ       :  �ɹ�����0 	
****************************************************************************
*/
int32_t web_post_channel_update_info(int8_t *post_url);

int32_t	init_web_protoclo_resolve_func(void);
#endif

