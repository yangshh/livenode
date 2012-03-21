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
	uint32_t m_time_tick;    //时间戳
	uint32_t m_frame_length; //帧数据长度
	uint32_t m_data_codec;   //编码方式
    uint32_t m_frame_rate;   //数据帧率 或 音频采样率
	uint32_t m_width;       //宽度	
	uint32_t m_hight;       //高度
	uint32_t m_colors;      //颜色数
	uint32_t m_dw_segment;		//组包标示 0表示中间包 1表示结尾包 2表示开始包 3表示独立包
	uint32_t m_dw_flags;			//帧标志 I帧？ 
	uint32_t m_dw_packet_number; 	//
	uint32_t m_others;      			//包对应窗口序号
} hdb_freame_head_t;

#define FH_LEN	sizeof(hdb_freame_head_t)
#endif
typedef struct msg_headers {//消息头
	uint16_t m_len;		//长度
	uint16_t m_ver;		//版本
	uint16_t m_msg_type;	//消息类型
	uint16_t m_data;	//保留
}msg_header_t;
/*
	网络端区分的是不同的用户
	本地端区分的是不同的通道
*/
#define LOACAL_DATA_MSG				"/tmp/local_data_msg.str"	//创建虚拟网络通信的文件路径,用于和发送端通信
#define MSG_TYPE_LOGIN				1	//登录。MsgHeader + UserInfo.
#define MSG_TYPE_DATAREQ			14	//是否需要服务器发送媒体数据MsgHeader+会议室ID(BYTE)+是否需要媒体数据(BYTE) 1表示需要，0表示不需要。
#define MAX_CUR_TO_END		10
#define MSG_TYPE_FIREWALL  			7	//穿透防火墙 MsgHeader+四字节ID号
#define	EV_TIMEOUT	-1

#define USER_LOGIN_MAXUSERS	-2	//用户数满
#define USER_LOGIN_ERROR	-1	//协议不对
#define USER_LOGIN_VALID	1	//验证通过
#define USER_LOGIN_INVALID	0	//无此用户或密码失败

//web_protocol_resolve.c
#define WEB_TCP_LISTEN_PORT		(3008)		//监听web通信的端口
#define HTTP_ALARM_TIME           			 3	        //直播节点定时上报媒体中心时间间隔


#define USER_NAME               "reach"
#define USER_PASSWD     "reachplayer"
#define UNITE_PASSWD     "reachserver"

typedef enum USER_FLAGe
{
        USER_FLAG_PLAYER,               //?±2￥ó??§
        USER_FLAG_UNITE,                //??áaó??§
        USER_FLAG_MANAGER,              //?áòé1üàí??ì¨
        USER_FLAG_LCD,                  //LCD
        USER_FLAG_WEB,                  //WEB
        USER_FLAG_OTHER,                //????ía2?3ìDò
        USER_FLAG_DEC                   //?a???÷
} USER_FLAG;

typedef struct net_user_infos {
	int8_t uname[20];				//用户名
	int8_t upasswd[20];			//密码
	USER_FLAG uflag;	
} net_user_info_t;

#define	MAX_CH		1024
#define 	MSG_HEAD_LEN	sizeof(msg_header_t)


//向发送服务器部分发送数据信息
typedef struct channel_infos {
	uint16_t 	m_msg_type;		//消息类型,表示是音频还是视频数据
	int32_t		m_channel;		//教室编号,即通道标号
	void*	m_data;			//数据地址的指针
	uint16_t	m_data_len;		//数据长度
	uint16_t	m_msg_flag;
} channel_info_t;

#define	CI_LEN	sizeof(channel_info_t)

void pack_header_msg(void *data, uint8_t type, uint16_t len);
void pack_unite_user_info(net_user_info_t *nu);
int32_t check_net_user(net_user_info_t *nu);
int write_file(char *file, void *buf, int buflen);
void init_channel_infos(channel_info_t *cit);
#endif	//__MEDIA_MSG_H__

