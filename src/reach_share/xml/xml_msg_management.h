/*******************************************************
 * Copyright (C), 2011-2018, Shenzhen Reach IT Co., Ltd.
 * @file:     xml_msg_management.h
 * @author:   丁兆柱
 * @version:     1.0   
 * @date:   2011-12-22
 * @brief:   xml 格式字符串封装和解析
 * @Others: 依赖 libxml2-2.5.6forlinux
 * @Function List: 
 * @History:    
 *  1. Date:
 *      Author:
 *     Modification:
 *******************************************************/

#ifndef __XML_MSG__
#define __XML_MSG__

/** @ingroup     */
/*@{*/

#include "xml_base.h"
/**
* @brief 存储xml格式字符串buffer最大值
*/
#define MAX_XML_LEN	(100*1024)
/**
* @brief 存储xml格式通道信息buffer的最大值
*/
#define ROOM_INFO_XML_LEN	256
/**
* @brief 存储验证码buffer大小
*/
#define IP_ADDR_LEN	16
#define PASS_KEY_LEN	128
#define ROOM_ID_MAX	10
#define REQ_CODE_TCP_SIZE  6
#define REQ_VALUE_KEY  10

#define IM_TO_MC_GET_RECSERVERS_INFO_XML_REQ_EMPTY "<?xml version=\"1.0\" encoding=\"UTF-8\"?><RequestMsg><MsgHead><MsgCode>%d</MsgCode><PassKey>%s</PassKey></MsgHead><MsgBody><LiverChannelUpdateReq></LiverChannelUpdateReq></MsgBody></RequestMsg>"

//#define IM_TO_MC_REPORT_LIVE_USERS_INFO_XML_REQ_EMPTY "<?xml version=\"1.0\" encoding=\"UTF-8\"?><RequestMsg><MsgHead><MsgCode>%d</MsgCode><PassKey>%s</PassKey></MsgHead><MsgBody><LiveUpdateReq><LiveIP>%s</LiveIP><BandWidth>%d</BandWidth><UserNum>%d</UserNum><UserIPs>%s</UserIPs></LiveUpdateReq></MsgBody></RequestMsg>"

#define IM_LIVE_CHANNEL_XML_RESP "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody><GetLiveChannelResp><ChannelID>%d</ChannelID></GetLiveChannelResp></MsgBody></ResponseMsg>"

#define IM_LIVE_USERS_INFO_XML_RESP "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody><GetLiveUsersInfoResp><BandWidth>%d</BandWidth><UserNum>%d</UserNum><UserIPs>%s</UserIPs></GetLiveUsersInfoResp></MsgBody></ResponseMsg>"

#define IM_LIVE_CHANNEL_UPDATE_XML_RESP "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody><SetRecServerChannelResp><RecServerIP>%s</RecServerIP>%s</SetRecServerChannelResp></MsgBody></ResponseMsg>"

#define IM_ROOM_INFO_XML "<RoomInfo><RoomID>%d</RoomID><Status>%d</Status><ChannelID>%d</ChannelID></RoomInfo>"

#define IM_SYS_INFO_XML_RESP "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody><GetSysInfoResp ><CPU>%d</CPU><Ram>%d</Ram><Network>%d<NetWork></GetSysInfoResp></MsgBody></ResponseMsg>"

#define IM_RETURN_CODE_XML_RESP "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody></MsgBody></ResponseMsg>"

#define IM_TO_MC_GET_RECSERVERS_INFO_XML_REQ "<?xml version=\"1.0\" encoding=\"UTF-8\"?><RequestMsg><MsgHead><MsgCode>%d</MsgCode><PassKey>%s</PassKey></MsgHead><MsgBody></MsgBody></RequestMsg>"

#define IM_TO_MC_REPORT_LIVE_USERS_INFO_XML_REQ "<?xml version=\"1.0\" encoding=\"UTF-8\"?><RequestMsg><MsgHead><MsgCode>%d</MsgCode><PassKey>%s</PassKey></MsgHead><MsgBody><LiveUpdateReq><LiveIP>%s</LiveIP><BandWidth>%d</BandWidth><UserNum>%d</UserNum><UserIPs>%s</UserIPs></LiveUpdateReq></MsgBody></RequestMsg>"

#define IM_TO_MC_REPORT_LIVE_CHANNEL_UPDATE_XML_REQ "<?xml version=\"1.0\" encoding=\"UTF-8\"?><RequestMsg><MsgHead><MsgCode>%d</MsgCode><PassKey>%s</PassKey></MsgHead><MsgBody><LiverChannelUpdateReq>%s</LiverChannelUpdateReq ></MsgBody></RequestMsg>"

// 新添TCP 20002 信令功能
#define IM_TO_MC_REPORT_LIVE_CHANNEL_UPDATE_XML_REQ_TCP  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody><GetLiveChannelResp>%s</GetLiveChannelResp></MsgBody></ResponseMsg>"
#define IM_TO_MC_REPORT_LIVE_CHANNEL_UPDATE_XML_REQ_TCP_EMPTY  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody><GetLiveChannelResp></GetLiveChannelResp></MsgBody></ResponseMsg>"

#define IM_TO_MC_REPORT_IM_ROOM_INFO_XML	"<RoomInfo><RecServerIP>%s</RecServerIP><RoomID>%d</RoomID><Status>%d</Status><ChannelID>%d</ChannelID></RoomInfo>"
/**
* @brief 响应消息头信息
*/
typedef struct resp_msg_heads {
	/** @0 --失败|1 --成功*/
	int return_code;
} resp_msg_head_t;
/**
* @brief 接收中心TCP请求消息头信息
*/
typedef struct req_msg_heads{
	char req_code_tcp[REQ_CODE_TCP_SIZE];  //zhengyb
	char req_value_key[REQ_VALUE_KEY]; // 暂时赋值为空 后备 大小定位10
}req_msg_head_t;

/**
* @brief 接收中心TCP请求消息channels   
	注意此结构与下面的 recserver_infos结构
	二者结构内部相同 不同的只是应用环境而已 
*/
typedef struct req_msg_serv_channels
{
	char rec_server_ip[IP_ADDR_LEN];
	int32_t rec_room_id[ROOM_ID_MAX];   //  
	int32_t ChannelID ; 
	char user_ip[IP_ADDR_LEN];
}req_msg_serv_channel_t;
/**
* @brief获取在线用户信息状态结构
*/
typedef struct resp_live_user_info
{
	int32_t BandWidth ;
	int32_t UserNum;
	int8_t *UserIPs;
}resp_live_user_info_t;

/**
* @brief获取PC系统信息状态结构
*/
typedef struct resp_pc_info
{
	int cpu_used ;  // CPU占用率
	int cpu_net  ;   // 网络占用率
	int cpu_mem ; // 内存占用率
}resp_pc_info_t;
/**
* @brief 上报消息头信息
*/
typedef struct to_mc_msg_heads {
	/** @上报消息码*/
	int msg_code;	
	/** @上报消息验证码*/
	char pass_key[PASS_KEY_LEN]; 
} to_mc_msg_head_t;

/**
* @brief  直播通道信息
*/
typedef struct live_channel_resps {
	/** @0 --失败|1 --成功*/
	int return_code;	
	/** @直播节点上对应的通道ID  1~9 , 返回-1表示该通道未连接*/
	int channel_id; 	
} live_channel_resp_t;

/**
* @brief  直播用户信息
*/
typedef struct live_users_info_resps {
	/** @0 --失败|1 --成功*/
	int return_code;	
	/** @总带宽*/
	int total_bandwidth; 
	 /** @总用户数*/
	int total_users; 	
	/** @用户IP1|用户IP2.|...用户IPn*/
	char *ip_pool; 		 
} live_users_info_resp_t;

/**
* @brief 通道信息
*/
typedef struct room_infos {
	/** @ 录播IP的通道ID */
	int recserver_channel_id;			
	/** @连接状态：1表示连接，0表示断开*/
	int recserver_connect_status;	
	/** @对应直播节点上的通道ID*/
	int channel_id;				
} room_info_t;

/**
* @brief  更新的直播通道信息
*/
typedef struct live_channel_update_resps {
	/** @0 --失败|1 --成功*/
	int return_code; 					
	/** @录播IP地址*/
	char rec_server_ip[IP_ADDR_LEN]; 	
	/** @总教室数(通道数)*/
	int total_rooms;					
	/** @通道信息*/
	room_info_t *all_room_info; 				
} live_channel_update_resp_t;

/**
* @brief 
*/
typedef struct to_mc_report_room_infos {
	/** @录播IP地址*/
	char rec_server_ip[IP_ADDR_LEN]; 	
	/** @RoomInfo xml字符串*/
	room_info_t room_info; 				
} to_mc_report_room_info_t;

/**
* @brief 向媒体中心上报的更新直播通道信息请求内容
*/
typedef struct to_mc_report_live_channel_update_reqs {
	/** @上报的消息头*/
	to_mc_msg_head_t to_mc_msg_head;				
	/** @总教室数(通道数)*/
	int total_report_rooms;							
	/** @RoomInfo xml字符串*/
	to_mc_report_room_info_t *to_mc_report_room_info; 
} to_mc_report_live_channel_update_req_t;

/**
* @brief 向媒体中心应答TCP 请求发送全部直播通道信息
*/
typedef struct to_mc_report_live_channel_update_reqs_tcp {
	/** @上报的消息头*/
	int returncode;				
	/** @总教室数(通道数)*/
	int total_report_rooms;							
	/** @RoomInfo xml字符串*/
	to_mc_report_room_info_t *to_mc_report_room_info; 
} to_mc_report_live_channel_update_req_tcp_t;

/**
* @brief 直播节点服务器的系统信息
*/
typedef struct sys_info_resps {
	/** @0 --失败|1 --成功*/
	int return_code;	
	/** @cpu占用率*/
	int per_cpu; 		
	/** @内存使用率*/
	int per_ram;		
	/** @网络占用率*/
	int per_network;	
} sys_info_t;

/**
* @brief 到媒体中心获取录播服务器信息请求内容
*/
typedef struct to_mc_get_recservers_info_reqs {
	/** @向媒体中心发送请求消息头*/
	to_mc_msg_head_t to_mc_msg_head; 
} to_mc_get_recservers_info_req_t;

typedef struct report_live_users_infos {
	/** @直播节点IP*/
	char node_server_ip[IP_ADDR_LEN];
	/** @总带宽*/
	int total_bandwidth; 	
	/** @总用户数*/
	int total_users; 	 	
	/** @用户IP1|用户IP2.|...用户IPn*/
	char *ip_pool; 		 	
} report_live_users_info_t;

/**
* @brief 向媒体中心上报的直播用户信息请求内容
*/
typedef struct to_mc_report_live_users_info_reqs {
	/** @上报的消息头*/
	to_mc_msg_head_t to_mc_msg_head;			
	/** @上报直播用户的信息*/	
	report_live_users_info_t report_live_users_info;	
} to_mc_report_live_users_info_req_t;

/**
* @brief 将直播通道信息封装成xml格式字符串
* @param  [OUT] xml 装xml字符串buf
* @param  [IN] live_channel 直播通道信息
* @exception  none
* @return 成功返回0
* @note none
* @remarks none
*/

int cap_to_mc_live_channel_empty_xml_resp(char *xml, to_mc_get_recservers_info_req_t to_mc_get_recservers_info);
int cap_to_mc_report_live_channel_update_xml_req_tcp_empty(char *xml, to_mc_report_live_channel_update_req_tcp_t to_mc_report_live_channel_update_tcp);
int cap_live_channel_xml_resp(char *xml, live_channel_resp_t live_channel);
int cap_to_mc_report_live_channel_update_xml_req_tcp(char *xml, to_mc_report_live_channel_update_req_tcp_t to_mc_report_live_channel_update_tcp);
/**
* @brief 将直播用户信息封装成xml格式字符串
* @param  [OUT] xml 装xml字符串buf
* @param  [IN] live_users_info 直播用户信息
* @exception  none
* @return 成功返回0
* @note none
* @remarks none
*/
int cap_live_users_info_xml_resp(char *xml, live_users_info_resp_t live_users_info);

/**
* @brief 将更新的直播通道信息封装成xml格式字符串
* @param  [OUT] xml 装xml字符串buf
* @param  [IN] live_channel_update 更新的直播通道信息
* @exception  none
* @return 成功返回0
* @note none
* @remarks none
*/
int cap_live_channel_update_xml_resp(char *xml, live_channel_update_resp_t live_channel_update);

/**
* @brief 将直播节点服务器的系统信息封装成xml格式字符串
* @param  [OUT] xml 装xml字符串buf
* @param  [IN] sys_info 直播节点服务器的系统信息
* @exception  none
* @return 成功返回0
* @note none
* @remarks none
*/
int cap_sys_info_xml_resp(char *xml, sys_info_t sys_info);

/**
* @brief 将响应消息的返回码封装成xml格式字符串
* @param  [OUT] xml 装xml字符串buf
* @param  [IN] return_code 响应消息的返回码
* @exception  none
* @return 成功返回0
* @note none
* @remarks none
*/
int cap_return_code_xml_resp(char *xml, int return_code);

/**
* @brief 将到媒体中心获取录播服务器信息请求封装成xml格式字符串
* @param  [OUT] xml 装xml字符串buf
* @param  [IN] to_mc_get_recservers_info 到媒体中心获取录播服务器信息请求内容
* @exception  none
* @return 成功返回0
* @note none
* @remarks none
*/
int cap_to_mc_get_recservers_info_xml_req(char *xml, to_mc_get_recservers_info_req_t to_mc_get_recservers_info);

/**
* @brief 将向媒体中心上报的直播用户信息请求封装成xml格式字符串
* @param  [OUT] xml 装xml字符串buf
* @param  [IN] to_mc_report_live_users_info 向媒体中心上报的直播用户信息请求内容
* @exception  none
* @return 成功返回0
* @note none
* @remarks none
*/
int cap_to_mc_report_live_users_info_xml_req(char *xml, to_mc_report_live_users_info_req_t to_mc_report_live_users_info);

/**
* @brief 将向媒体中心上报的更新直播通道信息请求封装成xml格式字符串
* @param  [OUT] xml 装xml字符串buf
* @param  [IN] to_mc_report_live_channel_update 向媒体中心上报的更新直播通道信息请求内容
* @exception  none
* @return 成功返回0
* @note none
* @remarks none
*/
int cap_to_mc_report_live_channel_update_xml_req(char *xml, to_mc_report_live_channel_update_req_t to_mc_report_live_channel_update);


/*###############################################*/


/**
* @brief 判断当前根节点是否是响应消息的根节点
* @param  [IN] proot 当前根节点
* @exception  none
* @return  是返回1 ，否返回0
* @note  none
* @remarks  none
*/
int is_resp_msg(xmlNodePtr  proot);

/**
* @brief 获取响应消息返回码节点
* @param  [OUT] *pnode 响应消息返回码节点
* @param  [IN] proot 响应消息返回码节点dom树根节点
* @exception  none
* @return  成功返回响应消息返回码节点，失败返回NULL
* @note  none
* @remarks  none
* @code
*	#include "xml_msg_management.h"
*	#define MSGHEAD "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>1</ReturnCode><ReturnCode>2</ReturnCode><ReturnCode>3</ReturnCode><ReturnCode>4</ReturnCode></MsgHead><MsgBody/></ResponseMsg>"
*	char value[8] = {0};
*	xmlNodePtr pnode;
*	parse_xml_t parse_xml;
*	init_dom_tree(&parse_xml, MSGHEAD);
*	get_resp_return_code_node(&pnode, parse_xml.proot);
*	get_current_node_value(value,value_len, parse_xml.pdoc, pnode);
*	printf("msg_head:[%s]\n", value);
*	release_dom_tree(parse_xml.pdoc);
* @endcode
*/
xmlNodePtr get_resp_return_code_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取响应消息录播服务器信息节点
* @param  [OUT] *pnode 响应消息录播服务器信息节点
* @param  [IN] proot 响应消息录播服务器信息节点所在dom树根节点
* @exception  none
* @return  成功返回响应消息录播服务器信息节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_resp_recserver_info_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取响应消息录播服务器IP 节点
* @param  [OUT] *pnode 响应消息录播服务器IP 节点
* @param  [IN] proot 响应消息录播服务器信息节点
* @exception  none
* @return  成功返回响应消息录播服务器IP 节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_resp_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr recserver_info_node);

/**
* @brief 获取响应消息通道ID  节点
* @param  [OUT] *pnode 响应消息录通道ID  节点
* @param  [IN] recserver_info_node  响应消息录播服务器信息节点
* @exception  none
* @return  成功返回响应消息录通道ID   节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_resp_room_id_node(xmlNodePtr *pnode, xmlNodePtr recserver_info_node);

typedef struct recserver_infos {
	char recserver_ip[IP_ADDR_LEN];
	int32_t room_id_pool[ROOM_ID_MAX];
} recserver_info_t;

/**
* @brief 录播服务器信息
* @param  [OUT]  recserver_info 存储录播服务器信息buffer
* @param  [IN] pdoc 响应消息录播服务器信息节点所在dom树句柄
* @param  [IN] recservers_info_node 响应消息录播服务器信息节点
* @exception  none  
* @return 成功返回0
* @note  none
* @remarks  none
*/
int get_recserver_info(recserver_info_t *recserver_info, xmlDocPtr pdoc, xmlNodePtr recservers_info_node);

/**
* @brief 判断当前根节点是否是请求消息的根节点
* @param  [IN] proot 当前根节点
* @exception  none
* @return  是返回1 ，否返回0
* @note  none
* @remarks  none
*/
int is_req_msg(xmlNodePtr  proot);

/**
* @brief 获取请求消息码节点
* @param  [OUT] *pnode 请求消息码节点
* @param  [IN] proot 请求消息码节点所在dom树的根节点
* @exception  none
* @return  成功返回消息码节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_msg_code_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取请求消息验证码节点
* @param  [OUT] *pnode 请求消息验证码节点
* @param  [IN] proot 请求消息验证码节点所在dom树的根节点
* @exception  none
* @return  成功返回消息验证码节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_pass_key_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取设置录播服务器IP 节点
* @param  [OUT] *pnode 设置录播服务器IP 节点
* @param  [IN] proot 设置录播服务器IP 节点所在dom树的根节点
* @exception  none
* @return  成功返回设置录播服务器IP 节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_setRecServerChannel_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取设置录播服务器通道ID 节点
* @param  [OUT] *pnode 设置录播服务器通道ID 节点
* @param  [IN] proot 设置录播服务器通道ID 节点所在dom树的根节点
* @exception  none
* @return  成功返回设置录播服务器通道ID 节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_setRecServerChannel_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取直播通道的录播IP 节点
* @param  [OUT] *pnode 直播通道的录播IP 节点
* @param  [IN] proot 直播通道的录播IP 节点所在dom树的根节点
* @exception  none
* @return  成功返回 直播通道的录播IP 节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_getLiveChannel_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取直播通道ID 节点
* @param  [OUT] *pnode 直播通道ID 节点
* @param  [IN] proot 直播通道ID 节点所在dom树的根节点
* @exception  none
* @return  成功返回 直播通道ID 节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_getLiveChannel_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取停止用户直播的录播IP 节点
* @param  [OUT] *pnode 停止用户直播的录播IP 节点
* @param  [IN] proot 停止用户直播的录播IP 节点所在dom树的根节点
* @exception  none  
* @return  成功返回 停止用户直播的录播IP 节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_stopUserLiveReq_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取停止用户直播的用户IP 节点
* @param  [OUT] *pnode 停止用户直播的用户IP 节点
* @param  [IN] proot 停止用户直播的用户IP 节点所在dom树的根节点
* @exception  none  
* @return  成功返回 停止用户直播的用户IP 节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_stopUserLiveReq_user_ip_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief 获取停止用户直播通道ID 节点
* @param  [OUT] *pnode 停止用户直播通道ID 节点
* @param  [IN] proot 停止用户直播通道ID 节点所在dom树的根节点
* @exception  none  
* @return  成功返回 停止用户直播通道ID 节点，失败返回NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_stopUserLiveReq_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot);

/*@}*/
#endif //__XML_MSG__

