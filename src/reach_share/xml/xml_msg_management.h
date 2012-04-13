/*******************************************************
 * Copyright (C), 2011-2018, Shenzhen Reach IT Co., Ltd.
 * @file:     xml_msg_management.h
 * @author:   ������
 * @version:     1.0   
 * @date:   2011-12-22
 * @brief:   xml ��ʽ�ַ�����װ�ͽ���
 * @Others: ���� libxml2-2.5.6forlinux
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
* @brief �洢xml��ʽ�ַ���buffer���ֵ
*/
#define MAX_XML_LEN	(100*1024)
/**
* @brief �洢xml��ʽͨ����Ϣbuffer�����ֵ
*/
#define ROOM_INFO_XML_LEN	256
/**
* @brief �洢��֤��buffer��С
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

// ����TCP 20002 �����
#define IM_TO_MC_REPORT_LIVE_CHANNEL_UPDATE_XML_REQ_TCP  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody><GetLiveChannelResp>%s</GetLiveChannelResp></MsgBody></ResponseMsg>"
#define IM_TO_MC_REPORT_LIVE_CHANNEL_UPDATE_XML_REQ_TCP_EMPTY  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ResponseMsg><MsgHead><ReturnCode>%d</ReturnCode></MsgHead><MsgBody><GetLiveChannelResp></GetLiveChannelResp></MsgBody></ResponseMsg>"

#define IM_TO_MC_REPORT_IM_ROOM_INFO_XML	"<RoomInfo><RecServerIP>%s</RecServerIP><RoomID>%d</RoomID><Status>%d</Status><ChannelID>%d</ChannelID></RoomInfo>"
/**
* @brief ��Ӧ��Ϣͷ��Ϣ
*/
typedef struct resp_msg_heads {
	/** @0 --ʧ��|1 --�ɹ�*/
	int return_code;
} resp_msg_head_t;
/**
* @brief ��������TCP������Ϣͷ��Ϣ
*/
typedef struct req_msg_heads{
	char req_code_tcp[REQ_CODE_TCP_SIZE];  //zhengyb
	char req_value_key[REQ_VALUE_KEY]; // ��ʱ��ֵΪ�� �� ��С��λ10
}req_msg_head_t;

/**
* @brief ��������TCP������Ϣchannels   
	ע��˽ṹ������� recserver_infos�ṹ
	���߽ṹ�ڲ���ͬ ��ͬ��ֻ��Ӧ�û������� 
*/
typedef struct req_msg_serv_channels
{
	char rec_server_ip[IP_ADDR_LEN];
	int32_t rec_room_id[ROOM_ID_MAX];   //  
	int32_t ChannelID ; 
	char user_ip[IP_ADDR_LEN];
}req_msg_serv_channel_t;
/**
* @brief��ȡ�����û���Ϣ״̬�ṹ
*/
typedef struct resp_live_user_info
{
	int32_t BandWidth ;
	int32_t UserNum;
	int8_t *UserIPs;
}resp_live_user_info_t;

/**
* @brief��ȡPCϵͳ��Ϣ״̬�ṹ
*/
typedef struct resp_pc_info
{
	int cpu_used ;  // CPUռ����
	int cpu_net  ;   // ����ռ����
	int cpu_mem ; // �ڴ�ռ����
}resp_pc_info_t;
/**
* @brief �ϱ���Ϣͷ��Ϣ
*/
typedef struct to_mc_msg_heads {
	/** @�ϱ���Ϣ��*/
	int msg_code;	
	/** @�ϱ���Ϣ��֤��*/
	char pass_key[PASS_KEY_LEN]; 
} to_mc_msg_head_t;

/**
* @brief  ֱ��ͨ����Ϣ
*/
typedef struct live_channel_resps {
	/** @0 --ʧ��|1 --�ɹ�*/
	int return_code;	
	/** @ֱ���ڵ��϶�Ӧ��ͨ��ID  1~9 , ����-1��ʾ��ͨ��δ����*/
	int channel_id; 	
} live_channel_resp_t;

/**
* @brief  ֱ���û���Ϣ
*/
typedef struct live_users_info_resps {
	/** @0 --ʧ��|1 --�ɹ�*/
	int return_code;	
	/** @�ܴ���*/
	int total_bandwidth; 
	 /** @���û���*/
	int total_users; 	
	/** @�û�IP1|�û�IP2.|...�û�IPn*/
	char *ip_pool; 		 
} live_users_info_resp_t;

/**
* @brief ͨ����Ϣ
*/
typedef struct room_infos {
	/** @ ¼��IP��ͨ��ID */
	int recserver_channel_id;			
	/** @����״̬��1��ʾ���ӣ�0��ʾ�Ͽ�*/
	int recserver_connect_status;	
	/** @��Ӧֱ���ڵ��ϵ�ͨ��ID*/
	int channel_id;				
} room_info_t;

/**
* @brief  ���µ�ֱ��ͨ����Ϣ
*/
typedef struct live_channel_update_resps {
	/** @0 --ʧ��|1 --�ɹ�*/
	int return_code; 					
	/** @¼��IP��ַ*/
	char rec_server_ip[IP_ADDR_LEN]; 	
	/** @�ܽ�����(ͨ����)*/
	int total_rooms;					
	/** @ͨ����Ϣ*/
	room_info_t *all_room_info; 				
} live_channel_update_resp_t;

/**
* @brief 
*/
typedef struct to_mc_report_room_infos {
	/** @¼��IP��ַ*/
	char rec_server_ip[IP_ADDR_LEN]; 	
	/** @RoomInfo xml�ַ���*/
	room_info_t room_info; 				
} to_mc_report_room_info_t;

/**
* @brief ��ý�������ϱ��ĸ���ֱ��ͨ����Ϣ��������
*/
typedef struct to_mc_report_live_channel_update_reqs {
	/** @�ϱ�����Ϣͷ*/
	to_mc_msg_head_t to_mc_msg_head;				
	/** @�ܽ�����(ͨ����)*/
	int total_report_rooms;							
	/** @RoomInfo xml�ַ���*/
	to_mc_report_room_info_t *to_mc_report_room_info; 
} to_mc_report_live_channel_update_req_t;

/**
* @brief ��ý������Ӧ��TCP ������ȫ��ֱ��ͨ����Ϣ
*/
typedef struct to_mc_report_live_channel_update_reqs_tcp {
	/** @�ϱ�����Ϣͷ*/
	int returncode;				
	/** @�ܽ�����(ͨ����)*/
	int total_report_rooms;							
	/** @RoomInfo xml�ַ���*/
	to_mc_report_room_info_t *to_mc_report_room_info; 
} to_mc_report_live_channel_update_req_tcp_t;

/**
* @brief ֱ���ڵ��������ϵͳ��Ϣ
*/
typedef struct sys_info_resps {
	/** @0 --ʧ��|1 --�ɹ�*/
	int return_code;	
	/** @cpuռ����*/
	int per_cpu; 		
	/** @�ڴ�ʹ����*/
	int per_ram;		
	/** @����ռ����*/
	int per_network;	
} sys_info_t;

/**
* @brief ��ý�����Ļ�ȡ¼����������Ϣ��������
*/
typedef struct to_mc_get_recservers_info_reqs {
	/** @��ý�����ķ���������Ϣͷ*/
	to_mc_msg_head_t to_mc_msg_head; 
} to_mc_get_recservers_info_req_t;

typedef struct report_live_users_infos {
	/** @ֱ���ڵ�IP*/
	char node_server_ip[IP_ADDR_LEN];
	/** @�ܴ���*/
	int total_bandwidth; 	
	/** @���û���*/
	int total_users; 	 	
	/** @�û�IP1|�û�IP2.|...�û�IPn*/
	char *ip_pool; 		 	
} report_live_users_info_t;

/**
* @brief ��ý�������ϱ���ֱ���û���Ϣ��������
*/
typedef struct to_mc_report_live_users_info_reqs {
	/** @�ϱ�����Ϣͷ*/
	to_mc_msg_head_t to_mc_msg_head;			
	/** @�ϱ�ֱ���û�����Ϣ*/	
	report_live_users_info_t report_live_users_info;	
} to_mc_report_live_users_info_req_t;

/**
* @brief ��ֱ��ͨ����Ϣ��װ��xml��ʽ�ַ���
* @param  [OUT] xml װxml�ַ���buf
* @param  [IN] live_channel ֱ��ͨ����Ϣ
* @exception  none
* @return �ɹ�����0
* @note none
* @remarks none
*/

int cap_to_mc_live_channel_empty_xml_resp(char *xml, to_mc_get_recservers_info_req_t to_mc_get_recservers_info);
int cap_to_mc_report_live_channel_update_xml_req_tcp_empty(char *xml, to_mc_report_live_channel_update_req_tcp_t to_mc_report_live_channel_update_tcp);
int cap_live_channel_xml_resp(char *xml, live_channel_resp_t live_channel);
int cap_to_mc_report_live_channel_update_xml_req_tcp(char *xml, to_mc_report_live_channel_update_req_tcp_t to_mc_report_live_channel_update_tcp);
/**
* @brief ��ֱ���û���Ϣ��װ��xml��ʽ�ַ���
* @param  [OUT] xml װxml�ַ���buf
* @param  [IN] live_users_info ֱ���û���Ϣ
* @exception  none
* @return �ɹ�����0
* @note none
* @remarks none
*/
int cap_live_users_info_xml_resp(char *xml, live_users_info_resp_t live_users_info);

/**
* @brief �����µ�ֱ��ͨ����Ϣ��װ��xml��ʽ�ַ���
* @param  [OUT] xml װxml�ַ���buf
* @param  [IN] live_channel_update ���µ�ֱ��ͨ����Ϣ
* @exception  none
* @return �ɹ�����0
* @note none
* @remarks none
*/
int cap_live_channel_update_xml_resp(char *xml, live_channel_update_resp_t live_channel_update);

/**
* @brief ��ֱ���ڵ��������ϵͳ��Ϣ��װ��xml��ʽ�ַ���
* @param  [OUT] xml װxml�ַ���buf
* @param  [IN] sys_info ֱ���ڵ��������ϵͳ��Ϣ
* @exception  none
* @return �ɹ�����0
* @note none
* @remarks none
*/
int cap_sys_info_xml_resp(char *xml, sys_info_t sys_info);

/**
* @brief ����Ӧ��Ϣ�ķ������װ��xml��ʽ�ַ���
* @param  [OUT] xml װxml�ַ���buf
* @param  [IN] return_code ��Ӧ��Ϣ�ķ�����
* @exception  none
* @return �ɹ�����0
* @note none
* @remarks none
*/
int cap_return_code_xml_resp(char *xml, int return_code);

/**
* @brief ����ý�����Ļ�ȡ¼����������Ϣ�����װ��xml��ʽ�ַ���
* @param  [OUT] xml װxml�ַ���buf
* @param  [IN] to_mc_get_recservers_info ��ý�����Ļ�ȡ¼����������Ϣ��������
* @exception  none
* @return �ɹ�����0
* @note none
* @remarks none
*/
int cap_to_mc_get_recservers_info_xml_req(char *xml, to_mc_get_recservers_info_req_t to_mc_get_recservers_info);

/**
* @brief ����ý�������ϱ���ֱ���û���Ϣ�����װ��xml��ʽ�ַ���
* @param  [OUT] xml װxml�ַ���buf
* @param  [IN] to_mc_report_live_users_info ��ý�������ϱ���ֱ���û���Ϣ��������
* @exception  none
* @return �ɹ�����0
* @note none
* @remarks none
*/
int cap_to_mc_report_live_users_info_xml_req(char *xml, to_mc_report_live_users_info_req_t to_mc_report_live_users_info);

/**
* @brief ����ý�������ϱ��ĸ���ֱ��ͨ����Ϣ�����װ��xml��ʽ�ַ���
* @param  [OUT] xml װxml�ַ���buf
* @param  [IN] to_mc_report_live_channel_update ��ý�������ϱ��ĸ���ֱ��ͨ����Ϣ��������
* @exception  none
* @return �ɹ�����0
* @note none
* @remarks none
*/
int cap_to_mc_report_live_channel_update_xml_req(char *xml, to_mc_report_live_channel_update_req_t to_mc_report_live_channel_update);


/*###############################################*/


/**
* @brief �жϵ�ǰ���ڵ��Ƿ�����Ӧ��Ϣ�ĸ��ڵ�
* @param  [IN] proot ��ǰ���ڵ�
* @exception  none
* @return  �Ƿ���1 ���񷵻�0
* @note  none
* @remarks  none
*/
int is_resp_msg(xmlNodePtr  proot);

/**
* @brief ��ȡ��Ӧ��Ϣ������ڵ�
* @param  [OUT] *pnode ��Ӧ��Ϣ������ڵ�
* @param  [IN] proot ��Ӧ��Ϣ������ڵ�dom�����ڵ�
* @exception  none
* @return  �ɹ�������Ӧ��Ϣ������ڵ㣬ʧ�ܷ���NULL
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
* @brief ��ȡ��Ӧ��Ϣ¼����������Ϣ�ڵ�
* @param  [OUT] *pnode ��Ӧ��Ϣ¼����������Ϣ�ڵ�
* @param  [IN] proot ��Ӧ��Ϣ¼����������Ϣ�ڵ�����dom�����ڵ�
* @exception  none
* @return  �ɹ�������Ӧ��Ϣ¼����������Ϣ�ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_resp_recserver_info_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡ��Ӧ��Ϣ¼��������IP �ڵ�
* @param  [OUT] *pnode ��Ӧ��Ϣ¼��������IP �ڵ�
* @param  [IN] proot ��Ӧ��Ϣ¼����������Ϣ�ڵ�
* @exception  none
* @return  �ɹ�������Ӧ��Ϣ¼��������IP �ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_resp_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr recserver_info_node);

/**
* @brief ��ȡ��Ӧ��Ϣͨ��ID  �ڵ�
* @param  [OUT] *pnode ��Ӧ��Ϣ¼ͨ��ID  �ڵ�
* @param  [IN] recserver_info_node  ��Ӧ��Ϣ¼����������Ϣ�ڵ�
* @exception  none
* @return  �ɹ�������Ӧ��Ϣ¼ͨ��ID   �ڵ㣬ʧ�ܷ���NULL
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
* @brief ¼����������Ϣ
* @param  [OUT]  recserver_info �洢¼����������Ϣbuffer
* @param  [IN] pdoc ��Ӧ��Ϣ¼����������Ϣ�ڵ�����dom�����
* @param  [IN] recservers_info_node ��Ӧ��Ϣ¼����������Ϣ�ڵ�
* @exception  none  
* @return �ɹ�����0
* @note  none
* @remarks  none
*/
int get_recserver_info(recserver_info_t *recserver_info, xmlDocPtr pdoc, xmlNodePtr recservers_info_node);

/**
* @brief �жϵ�ǰ���ڵ��Ƿ���������Ϣ�ĸ��ڵ�
* @param  [IN] proot ��ǰ���ڵ�
* @exception  none
* @return  �Ƿ���1 ���񷵻�0
* @note  none
* @remarks  none
*/
int is_req_msg(xmlNodePtr  proot);

/**
* @brief ��ȡ������Ϣ��ڵ�
* @param  [OUT] *pnode ������Ϣ��ڵ�
* @param  [IN] proot ������Ϣ��ڵ�����dom���ĸ��ڵ�
* @exception  none
* @return  �ɹ�������Ϣ��ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_msg_code_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡ������Ϣ��֤��ڵ�
* @param  [OUT] *pnode ������Ϣ��֤��ڵ�
* @param  [IN] proot ������Ϣ��֤��ڵ�����dom���ĸ��ڵ�
* @exception  none
* @return  �ɹ�������Ϣ��֤��ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_pass_key_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡ����¼��������IP �ڵ�
* @param  [OUT] *pnode ����¼��������IP �ڵ�
* @param  [IN] proot ����¼��������IP �ڵ�����dom���ĸ��ڵ�
* @exception  none
* @return  �ɹ���������¼��������IP �ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_setRecServerChannel_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡ����¼��������ͨ��ID �ڵ�
* @param  [OUT] *pnode ����¼��������ͨ��ID �ڵ�
* @param  [IN] proot ����¼��������ͨ��ID �ڵ�����dom���ĸ��ڵ�
* @exception  none
* @return  �ɹ���������¼��������ͨ��ID �ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_setRecServerChannel_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡֱ��ͨ����¼��IP �ڵ�
* @param  [OUT] *pnode ֱ��ͨ����¼��IP �ڵ�
* @param  [IN] proot ֱ��ͨ����¼��IP �ڵ�����dom���ĸ��ڵ�
* @exception  none
* @return  �ɹ����� ֱ��ͨ����¼��IP �ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_getLiveChannel_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡֱ��ͨ��ID �ڵ�
* @param  [OUT] *pnode ֱ��ͨ��ID �ڵ�
* @param  [IN] proot ֱ��ͨ��ID �ڵ�����dom���ĸ��ڵ�
* @exception  none
* @return  �ɹ����� ֱ��ͨ��ID �ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_getLiveChannel_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡֹͣ�û�ֱ����¼��IP �ڵ�
* @param  [OUT] *pnode ֹͣ�û�ֱ����¼��IP �ڵ�
* @param  [IN] proot ֹͣ�û�ֱ����¼��IP �ڵ�����dom���ĸ��ڵ�
* @exception  none  
* @return  �ɹ����� ֹͣ�û�ֱ����¼��IP �ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_stopUserLiveReq_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡֹͣ�û�ֱ�����û�IP �ڵ�
* @param  [OUT] *pnode ֹͣ�û�ֱ�����û�IP �ڵ�
* @param  [IN] proot ֹͣ�û�ֱ�����û�IP �ڵ�����dom���ĸ��ڵ�
* @exception  none  
* @return  �ɹ����� ֹͣ�û�ֱ�����û�IP �ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_stopUserLiveReq_user_ip_node(xmlNodePtr *pnode, xmlNodePtr proot);

/**
* @brief ��ȡֹͣ�û�ֱ��ͨ��ID �ڵ�
* @param  [OUT] *pnode ֹͣ�û�ֱ��ͨ��ID �ڵ�
* @param  [IN] proot ֹͣ�û�ֱ��ͨ��ID �ڵ�����dom���ĸ��ڵ�
* @exception  none  
* @return  �ɹ����� ֹͣ�û�ֱ��ͨ��ID �ڵ㣬ʧ�ܷ���NULL
* @note  none
* @remarks  none
* @see  get_resp_return_code_node
*/
xmlNodePtr get_req_stopUserLiveReq_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot);

/*@}*/
#endif //__XML_MSG__

