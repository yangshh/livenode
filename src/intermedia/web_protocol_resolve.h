#ifndef __FILE_WEB_PROTOCOL_RESOLVE_H
#define __FILE_WEB_PROTOCOL_RESOLVE_H

#define WEB_G_IP_MSG				"/tmp/web_g_ip_msg.txt"

#include "connect_rec_server.h"
#include "xml_msg_management.h"
#include<semaphore.h>



#define	ONE_REPORT_MSG_LEN		(256)	//上报消息中每个通道的长度
#define		WEB_MSG_LEN			(ONE_REPORT_MSG_LEN * CHANNEL_CONNECT_MAX_NUM)	//web发送消息的长度
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
	int32_t	m_client_socket;	//连接上的客户端的socket
}web_client_info_t;

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

/**
* @brief 通过全局变量获取媒体中心地址
* @param  [OUT] ip 媒体中心地址
* @param  [IN] NULL
* @exception  -1
* @return 成功1 失败返回0
*/
int32_t get_web_url(char *ip);

/**
* @brief 获取本地网卡(ETH_NAME 宏定义网卡)的IP 为直播结点IP
* @param  [OUT]直播结点IP
* @param  [IN] NULL
* @exception  -1
* @return 成功0 失败返回-1
*/
int32_t fine_local_ip(char *ip);

/**
* @brief 直播结点循环上报执行函数
* @param  [OUT] NULL
* @param  [IN] NULL
* @exception  -1
* @return NULL
*/
void judge_user_channel_http_report();




/**
* @brief 遍历全局变量g_connect_info 寻求数组空闲元素
* @param  [OUT] g_connect_info 数组下标
* @param  [IN] NULL
* @exception  -1
* @return 成功返回数组下标值
*/
int32_t Traverse_gAmarry_free();

/**
* @brief 遍历全局变量g_connect_info 寻求处于连接状态元素
* @param  [OUT] g_connect_info 数组下标
* @param  [IN] NULL
* @exception  -1
* @return 成功返回数组下标值
*/
int32_t Traverse_gAmarry_usd();

/**
* @brief 处理直播结点与WEB 的msgcode_tcp_rec_serv TCP 协议
* @param  [OUT] p_buffer 封装后的上报信息
* @param  [IN] 解析WEB下发的TCP 请求信息
* @exception  -1
* @return 成功返回0
*/
int32_t msgcode_tcp_rec_serv_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/**
* @brief 处理直播结点与WEB 的msgcode_tcp_live_channel TCP 协议
* @param  [OUT] p_buffer 封装后的上报信息
* @param  [IN] 解析WEB下发的TCP 请求信息
* @exception  -1
* @return 成功返回0
*/
int32_t msgcode_tcp_live_channel_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/**
* @brief 处理直播结点与WEB 的msgcode_tcp_live_users TCP 协议
* @param  [OUT] p_buffer 封装后的上报信息
* @param  [IN] 解析WEB下发的TCP 请求信息
* @exception  -1
* @return 成功返回0
*/
int32_t msgcode_tcp_live_users_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/**
* @brief 处理直播结点与WEB 的 msgcode_tcp_sys_info TCP 协议
* @param  [OUT] p_buffer 封装后的上报信息
* @param  [IN] 解析WEB下发的TCP 请求信息
* @exception  -1
* @return 成功返回0
*/
int32_t msgcode_tcp_sys_info_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id);

/**
* @brief  封装http post 请求获取录播信息为xml 模式
* msgcode  -> MSGCODE_HTTP_REC_SERV
* @param  [OUT] 存放xml 模式录播信息的buffer 
* @param  [IN] int8_t *
* @exception  -1
* @return 成功返回0
*/
int32_t package_web_cmd_http_ServInfoReq(int8_t *buffer);

/**
* @brief  封装http post 上报直播在线用户信息为xml 模式
* msgcode  -> MSGCODE_HTTP_LIVE_USERS
* @param  [OUT] 存放xml 模式在线用户信息的buffer 
* @param  [IN] int8_t *
* @exception  -1
* @return 成功返回0
*/
int32_t package_web_cmd_http_LiveUpdateReq(int8_t *buffer);

/**
* @brief  封装http post 上报录播服务器通道信息为xml 模式
* msgcode  -> MSGCODE_HTTP_LIVE_CHANNEL
* @param  [OUT] 存放xml 模式录播服务器通道信息的buffer 
* @param  [IN] int8_t *
* @exception  -1
* @return 成功返回0
*/
int32_t package_web_cmd_http_LiverChannelUpdateReq(int8_t *buffer);

/**
* @brief  解析http post 协议1  返回信息并连接录播服务器 
* msgcode  -> MSGCODE_HTTP_REC_SERV
* @param  [OUT] NULL
* @param  [IN] 存放xml 模式录播信息的buffer 
* @exception  -1
* @return 成功返回0
*/
int32_t parse_web_cmd_http_ServInfoReq(int8_t *buffer);

/**
* @brief  解析http post 返回消息码判断上报用户与通道信息是否媒体中心正常接收 
* msgcode  -> MSGCODE_HTTP_LIVE_USERS and MSGCODE_HTTP_LIVE_CHANNEL
* @param  [OUT] NULL
* @param  [IN] 存放xml 模式post 上报后返回信息的buffer 
* @exception  -1
* @return 成功返回0
*/
int32_t parse_ResponseMsg(int8_t *buffer);

/**
* @brief  解析媒体中心下发xml模式TCP 请求信息填充为req_msg_serv_channel_t结构变量中
* msgcode  -> 所有TCP 请求模式
* @param  [OUT] 存放媒体中心下发TCP 请求信息的req_msg_serv_channel_t
* @param  [OUT]    req_msg_code 媒体中心下发TCP 请求类型码
* @param  [IN] 存放媒体中心下发xml模式TCP  请求信息
* @exception  -1
* @return 成功返回0
*/
int32_t	parse_web_cmd_tcp(req_msg_serv_channel_t* req_msg_room_id,int32_t* req_msg_code, int8_t 
*buffer);

/**
* @brief 定时以http模式上报直播在线用户信息
* msgcode  -> MSGCODE_HTTP_LIVE_USERS
* @param  [OUT] NULL
* @param  [IN] 媒体媒体中心IP 地址
* @exception  -1
* @return 成功返回0
*/
int32_t web_post_live_user_info(int8_t *post_url);

/**
* @brief  定时以http模式上报录播服务器通道信息
* msgcode  -> MSGCODE_HTTP_LIVE_CHANNEL
* @param  [OUT] NULL
* @param  [IN] 媒体媒体中心IP 地址
* @exception  -1
* @return 成功返回0
*/
int32_t web_post_channel_update_info(int8_t *post_url);

/**
* @brief  结点接收媒体中心TCP 请求的线程函数
* @param  [OUT] NULL
* @param  [IN] 连接上的客户端的socket
* @exception  NULL
* @return  NULL
*/
//static void* create_listen_client_thread(void* arg);

/**
* @brief  结点接收媒体中心http 请求的线程函数
* 此函数中对TCP 解析后的请求做出相应的处理!!!!!!
* @param  [OUT] NULL
* @param  [IN] NULL
* @exception  NULL
* @return  NULL
*/
//static void* create_http_report_thread(void* arg);

/**
* @brief  结点监听媒体中心tcp 连接的线程函数
* 在其中启动TCP 接收线程create_listen_client_thread 
* @param  [OUT] NULL
* @param  [IN] NULL
* @exception  NULL
* @return  NULL
*/
//static void* create_web_listen_thread(void* arg);

int32_t	init_web_protoclo_resolve_func(void);
#endif

