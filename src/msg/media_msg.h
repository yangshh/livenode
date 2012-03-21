#ifndef	__MEDIA_MSG_H__
#define	__MEDIA_MSG_H__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "stdint.h"
#define	T_FILE	"video_test.h264"
#if 1
/*UDP NetWork Header*/
typedef struct hdb_freame_heads
{
	uint32_t m_id;			//= mmioFOURCC('R','Q','H','D');
	uint32_t m_time_tick;    //ʱ���
	uint32_t m_frame_length; //֡���ݳ���
	uint32_t m_data_codec;   //���뷽ʽ
    uint32_t m_frame_rate;   //����֡�� �� ��Ƶ������
	uint32_t m_width;       //���	
	uint32_t m_hight;       //�߶�
	uint32_t m_colors;      //��ɫ��
	uint32_t m_dw_segment;		//�����ʾ 0��ʾ�м�� 1��ʾ��β�� 2��ʾ��ʼ�� 3��ʾ������
	uint32_t m_dw_flags;			//֡��־ I֡�� 
	uint32_t m_dw_packet_number; 	//
	uint32_t m_others;      			//����Ӧ�������
} hdb_freame_head_t;

#define FH_LEN	sizeof(hdb_freame_head_t)
#endif
typedef struct msg_headers {//��Ϣͷ
	uint16_t m_len;		//����
	uint16_t m_ver;		//�汾
	uint16_t m_msg_type;	//��Ϣ����
	uint16_t m_data;	//����
}msg_header_t;
/*
	��������ֵ��ǲ�ͬ���û�
	���ض����ֵ��ǲ�ͬ��ͨ��
*/
#define LOACAL_DATA_MSG				"/tmp/local_data_msg.str"	//������������ͨ�ŵ��ļ�·��,���ںͷ��Ͷ�ͨ��
#define MSG_TYPE_LOGIN				1	//��¼��MsgHeader + UserInfo.
#define MSG_TYPE_DATAREQ			14	//�Ƿ���Ҫ����������ý������MsgHeader+������ID(BYTE)+�Ƿ���Ҫý������(BYTE) 1��ʾ��Ҫ��0��ʾ����Ҫ��
#define MAX_CUR_TO_END		10
#define MSG_TYPE_FIREWALL  			7	//��͸����ǽ MsgHeader+���ֽ�ID��
#define	EV_TIMEOUT	-1

#define USER_LOGIN_MAXUSERS	-2	//�û�����
#define USER_LOGIN_ERROR	-1	//Э�鲻��
#define USER_LOGIN_VALID	1	//��֤ͨ��
#define USER_LOGIN_INVALID	0	//�޴��û�������ʧ��

//web_protocol_resolve.c
#define WEB_TCP_LISTEN_PORT		(3008)		//����webͨ�ŵĶ˿�
#define HTTP_ALARM_TIME           			 3	        //ֱ���ڵ㶨ʱ�ϱ�ý������ʱ����


#define USER_NAME               "reach"
#define USER_PASSWD     "reachplayer"
#define UNITE_PASSWD     "reachserver"

typedef enum USER_FLAGe
{
        USER_FLAG_PLAYER,               //?��2����??��
        USER_FLAG_UNITE,                //??��a��??��
        USER_FLAG_MANAGER,              //?������1������??����
        USER_FLAG_LCD,                  //LCD
        USER_FLAG_WEB,                  //WEB
        USER_FLAG_OTHER,                //????��a2?3��D��
        USER_FLAG_DEC                   //?a???��
} USER_FLAG;

typedef struct net_user_infos {
	int8_t uname[20];				//�û���
	int8_t upasswd[20];			//����
	USER_FLAG uflag;	
} net_user_info_t;

#define	MAX_CH		1024
#define 	MSG_HEAD_LEN	sizeof(msg_header_t)


//���ͷ��������ַ���������Ϣ
typedef struct channel_infos {
	uint16_t 	m_msg_type;		//��Ϣ����,��ʾ����Ƶ������Ƶ����
	int32_t		m_channel;		//���ұ��,��ͨ�����
	void*	m_data;			//���ݵ�ַ��ָ��
	uint16_t	m_data_len;		//���ݳ���
	uint16_t	m_msg_flag;
} channel_info_t;

#define	CI_LEN	sizeof(channel_info_t)

void pack_header_msg(void *data, uint8_t type, uint16_t len);
void pack_unite_user_info(net_user_info_t *nu);
int32_t check_net_user(net_user_info_t *nu);
int write_file(char *file, void *buf, int buflen);
void init_channel_infos(channel_info_t *cit);
#endif	//__MEDIA_MSG_H__

