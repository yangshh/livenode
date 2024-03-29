#ifndef __FILE_WEB_PROTOCOL_RESOLVE_H
#define __FILE_WEB_PROTOCOL_RESOLVE_H

#define WEB_G_IP_MSG						"/tmp/web_g_ip_msg.txt"

#include "xml_msg_management.h"
#include <semaphore.h>

#define	ONE_REPORT_MSG_LEN				(256)	//上报消息中每个通道的长度
#define		WEB_MSG_LEN				(ONE_REPORT_MSG_LEN * CHANNEL_CONNECT_MAX_NUM)	//web发送消息的长度
#define 	WEB_TEMP_MSG_LEN				  10

// 直播结点与媒体中心交互HTTP 协议 MsgCode
#define    MSGCODE_HTTP_REC_SERV                     10001
#define    MSGCODE_HTTP_LIVE_USERS                  10002
#define    MSGCODE_HTTP_LIVE_CHANNEL       	  10003
// 直播结点与媒体中心交互TCP 协议 MsgCode
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
	sem_t m_sem;	//创建线程时的信号量,用于等待一个客户端创建成功后再创建第二个
}web_global_vaiables_t;

// zhengyb add 12_2_2

/*
****************************************************************************
**函数名称: get_web_url()
**函数功能: 通过全局变量g_web_ip 获取媒体中心地址
**输入参数: NULL	
**输出参数:  ip ---媒体中心地址
**返回值       :  成功返回0 	失败返回-1
****************************************************************************
*/
int32_t get_web_url(char *ip);

/*
****************************************************************************
**函数名称: set_wet_url()
**函数功能: 每次媒体中心连接直播结点需要对媒体中心地址进行判别处理
**输入参数:  ip---媒体中心地址	
**输出参数:  NULL
**返回值       :  成功返回0 	失败返回-1
****************************************************************************
*/
int32_t set_wet_url(char *ip,void *vag);

/*
****************************************************************************
**函数名称: judge_user_channel_http_report()
**函数功能: 直播结点循环上报执行函数
**输入参数: 	NULL
**输出参数:    NULL
**返回值      :    NULL
****************************************************************************
*/
void judge_user_channel_http_report();

/*
****************************************************************************
**函数名称: Traverse_gAmarry_free()
**函数功能: 遍历全局变量g_connect_info 寻求数组空闲元素
**输入参数: 	NULL
**输出参数:    NULL
**返回值       :   成功返回数组下标值 	失败返回-1
****************************************************************************
*/
int32_t Traverse_gAmarry_free();

/*
****************************************************************************
**函数名称: Traverse_gAmarry_usd()
**函数功能: 遍历全局变量g_connect_info 寻求处于连接状态元素
**输入参数: 	NULL
**输出参数:    NULL
**返回值      :   成功返回数组下标值 	失败返回-1
****************************************************************************
*/
int32_t Traverse_gAmarry_usd();

/*
****************************************************************************
**函数名称: msgcode_tcp_rec_serv_func()
**函数功能: 处理直播结点与WEB 的msgcode_tcp_rec_serv TCP 协议
**输入参数: 	req_msg_room_id ---解析WEB下发的TCP 请求信息
**输出参数:   p_buffer ---封装后的上报信息
**返回值       :  成功返回0 	失败返回-1
****************************************************************************
*/
int32_t msgcode_tcp_rec_serv_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/*
****************************************************************************
**函数名称: msgcode_tcp_live_channel_func()
**函数功能: 处理直播结点与WEB 的msgcode_tcp_rec_serv TCP 协议
**输入参数: 	req_msg_room_id ---解析WEB下发的TCP 请求信息
**输出参数:   p_buffer ---封装后的上报信息
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t msgcode_tcp_live_channel_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/*
****************************************************************************
**函数名称: msgcode_tcp_live_users_func()
**函数功能: 处理直播结点与WEB 的msgcode_tcp_live_users TCP 协议�
**输入参数: 	req_msg_room_id ---解析WEB下发的TCP 请求信息
**输出参数:   p_buffer ---封装后的上报信息
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t msgcode_tcp_live_users_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/*
****************************************************************************
**函数名称: msgcode_tcp_sys_info_func()
**函数功能:  处理直播结点与WEB 的 msgcode_tcp_sys_info TCP 协议
**输入参数: 	req_msg_room_id ---解析WEB下发的TCP 请求信息
**输出参数:   p_buffer ---封装后的上报信息
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t msgcode_tcp_sys_info_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/*
****************************************************************************
**函数名称: package_web_cmd_http_ServInfoReq()
**函数功能:  封装获取录播信息的HTTP 上报请求为xml 模式
**输入参数:  NULL
**输出参数:   p_buffer ---封装后的上报信息
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t package_web_cmd_http_ServInfoReq(int8_t *buffer);

/*
****************************************************************************
**函数名称: package_web_cmd_http_LiveUpdateReq()
**函数功能:  封装直播结点在线用户信息为HTTP上报信息 (xml 模式)
**输入参数:  NULL
**输出参数:   p_buffer ---封装后的上报信息
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t package_web_cmd_http_LiveUpdateReq(int8_t *buffer);

/*
****************************************************************************
**函数名称: package_web_cmd_http_LiverChannelUpdateReq()
**函数功能:  封装直播结点通道信息为HTTP上报信息 (xml 模式)
**输入参数:  NULL
**输出参数:   p_buffer ---封装后的上报信息
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t package_web_cmd_http_LiverChannelUpdateReq(int8_t *buffer);

/*
****************************************************************************
**函数名称: parse_web_cmd_http_ServInfoReq()
**函数功能:  解析获取录播信息HTTP 协议返回信息并连接录播服务器 
**输入参数:  存放xml 模式post 上报后返回信息的buffer 
**输出参数:   NULL
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t parse_web_cmd_http_ServInfoReq(int8_t *buffer);

/*
****************************************************************************
**函数名称: parse_ResponseMsg()
**函数功能:  解析HTTP 协议返回码判断直播结点与媒体中心通讯是否正常
			       msgcode  -> MSGCODE_HTTP_LIVE_USERS and MSGCODE_HTTP_LIVE_CHANNEL
**输入参数:  存放xml 模式post 上报后返回信息的buffer 
**输出参数:   NULL
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t parse_ResponseMsg(int8_t *buffer);

/*
****************************************************************************
**函数名称:  parse_web_cmd_tcp()
**函数功能:  解析媒体中心下发xml模式TCP 请求信息填充为req_msg_serv_channel_t结构变量中
			       msgcode  -> 所有TCP 请求模式
**输入参数:  buffer ---存放媒体中心下发xml模式TCP  请求信息
**输出参数:  req_msg_code  ---媒体中心下发TCP 请求类型码
			       req_msg_room_id ---存放媒体中心下发TCP 请求信息
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t	parse_web_cmd_tcp(req_msg_serv_channel_t* req_msg_room_id,int32_t* req_msg_code, int8_t 
*buffer);

/*
****************************************************************************
**函数名称: web_post_live_user_info()
**函数功能:  定时以http模式上报直播在线用户信息
**输入参数:  post_url ---媒体媒体中心IP 地址
**输出参数:   NULL
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t web_post_live_user_info(int8_t *post_url);

/*
****************************************************************************
**函数名称: web_post_channel_update_info()
**函数功能:  定时以http模式上报录播服务器通道信息
**输入参数:  post_url ---媒体媒体中心IP 地址
**输出参数:   NULL
**返回值       :  成功返回0 	
****************************************************************************
*/
int32_t web_post_channel_update_info(int8_t *post_url);

int32_t	init_web_protoclo_resolve_func(void);
#endif

