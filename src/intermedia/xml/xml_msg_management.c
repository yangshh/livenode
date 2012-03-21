/*******************************************************
 * Copyright (C), 2011-2018, Shenzhen Reach IT Co., Ltd.
 * @file:     xml_msg_management.c
 * @author:   ¶¡Õ×Öù
 * @version:     1.0   
 * @date:   2011-12-22
 * @brief:   xml ¸ñÊ½×Ö·û´®·â×°ºÍ½âÎö
 * @Others: ÒÀÀµ libxml2-2.5.6forlinux
 * @Function List: 
 * @History:    
 *  1. Date:
 *      Author:
 *     Modification:
 *******************************************************/
 #include <stdlib.h>
#include "xml_msg_management.h"

#define RESP_ROOT_KEY (BAD_CAST "ResponseMsg")
#define REQ_ROOT_KEY (BAD_CAST "RequestMsg")


int cap_to_mc_live_channel_empty_xml_resp(char *xml, to_mc_get_recservers_info_req_t to_mc_get_recservers_info)
{
	r_snprintf(xml, MAX_XML_LEN -1, IM_TO_MC_GET_RECSERVERS_INFO_XML_REQ_EMPTY,
			to_mc_get_recservers_info.to_mc_msg_head.msg_code,
			to_mc_get_recservers_info.to_mc_msg_head.pass_key);
	return 0;
}

int cap_live_channel_xml_resp(char *xml, live_channel_resp_t live_channel)
{
	r_snprintf(xml, MAX_XML_LEN-1, IM_LIVE_CHANNEL_XML_RESP, 
			live_channel.return_code, 
			live_channel.channel_id);
	return 0;
}

int cap_live_users_info_xml_resp(char *xml, live_users_info_resp_t live_users_info)
{
//	printf("live_users_info of ip is  -- [%s]\n",&live_users_info.ip_pool[22]);
	r_snprintf(xml, MAX_XML_LEN-1, IM_LIVE_USERS_INFO_XML_RESP,
			live_users_info.return_code, 
			live_users_info.total_bandwidth, 
			live_users_info.total_users,
			live_users_info.ip_pool);
	return 0;
}

static int cap_room_info_xml(char *xml, room_info_t *all_room_info, int total_rooms)
{
	char tmp[ROOM_INFO_XML_LEN] = {0};
	while(total_rooms --) {
		r_snprintf(tmp, ROOM_INFO_XML_LEN-1, IM_ROOM_INFO_XML,
				all_room_info->recserver_channel_id,
				all_room_info->recserver_connect_status,
				all_room_info->channel_id);
		r_strcat(xml, tmp);
		all_room_info ++;
	}
	return 0;
}

int cap_live_channel_update_xml_resp(char *xml, live_channel_update_resp_t live_channel_update)
{
	char *all_room_info_xml = r_malloc(ROOM_INFO_XML_LEN * live_channel_update.total_rooms);
	r_memset(all_room_info_xml, 0, ROOM_INFO_XML_LEN * live_channel_update.total_rooms);
	cap_room_info_xml(all_room_info_xml, live_channel_update.all_room_info, live_channel_update.total_rooms);
	r_snprintf(xml, MAX_XML_LEN-1, IM_LIVE_CHANNEL_UPDATE_XML_RESP,
			live_channel_update.return_code,
			live_channel_update.rec_server_ip,
			all_room_info_xml);
	r_free(all_room_info_xml);
	return 0;
}

int cap_sys_info_xml_resp(char *xml, sys_info_t sys_info)
{
	r_snprintf(xml, MAX_XML_LEN-1, IM_SYS_INFO_XML_RESP,
			sys_info.return_code,
			sys_info.per_cpu,
			sys_info.per_ram,
			sys_info.per_network);
	return 0;
}

int cap_return_code_xml_resp(char *xml, int return_code)
{
	r_snprintf(xml, MAX_XML_LEN-1, IM_RETURN_CODE_XML_RESP, return_code);
	return 0;
}

int cap_to_mc_get_recservers_info_xml_req(char *xml, to_mc_get_recservers_info_req_t to_mc_get_recservers_info)
{
	r_snprintf(xml, MAX_XML_LEN -1, IM_TO_MC_GET_RECSERVERS_INFO_XML_REQ,
			to_mc_get_recservers_info.to_mc_msg_head.msg_code,
			to_mc_get_recservers_info.to_mc_msg_head.pass_key);
	return 0;
}

int cap_to_mc_report_live_users_info_xml_req(char *xml, to_mc_report_live_users_info_req_t to_mc_report_live_users_info)
{
	r_snprintf(xml, MAX_XML_LEN-1, IM_TO_MC_REPORT_LIVE_USERS_INFO_XML_REQ,
			to_mc_report_live_users_info.to_mc_msg_head.msg_code,
			to_mc_report_live_users_info.to_mc_msg_head.pass_key,
			to_mc_report_live_users_info.report_live_users_info.node_server_ip,
			to_mc_report_live_users_info.report_live_users_info.total_bandwidth,
			to_mc_report_live_users_info.report_live_users_info.total_users,
			to_mc_report_live_users_info.report_live_users_info.ip_pool);
	return 0;
}

static int cap_to_mc_report_room_info_xml(char *xml, to_mc_report_room_info_t *to_mc_report_room_info, int total_report_rooms)
{
	char tmp[ROOM_INFO_XML_LEN] = {0};
	while(total_report_rooms --) {
		r_snprintf(tmp, ROOM_INFO_XML_LEN-1, IM_TO_MC_REPORT_IM_ROOM_INFO_XML, 
				to_mc_report_room_info->rec_server_ip,
				to_mc_report_room_info->room_info.recserver_channel_id,
				to_mc_report_room_info->room_info.recserver_connect_status,
				to_mc_report_room_info->room_info.channel_id);
		r_strcat(xml, tmp);
		to_mc_report_room_info ++;
	}
	return 0;
}

int cap_to_mc_report_live_channel_update_xml_req(char *xml, to_mc_report_live_channel_update_req_t to_mc_report_live_channel_update)
{
	char *all_report_room_info_xml = r_malloc(ROOM_INFO_XML_LEN * to_mc_report_live_channel_update.total_report_rooms);
	r_memset(all_report_room_info_xml, 0, ROOM_INFO_XML_LEN * to_mc_report_live_channel_update.total_report_rooms);
	cap_to_mc_report_room_info_xml(all_report_room_info_xml, to_mc_report_live_channel_update.to_mc_report_room_info, to_mc_report_live_channel_update.total_report_rooms);
	r_snprintf(xml, MAX_XML_LEN-1, IM_TO_MC_REPORT_LIVE_CHANNEL_UPDATE_XML_REQ,
			to_mc_report_live_channel_update.to_mc_msg_head.msg_code,
			to_mc_report_live_channel_update.to_mc_msg_head.pass_key,
			all_report_room_info_xml);
	r_free(all_report_room_info_xml);
	return 0;
}

int cap_to_mc_report_live_channel_update_xml_req_tcp(char *xml, to_mc_report_live_channel_update_req_tcp_t to_mc_report_live_channel_update_tcp)
{
	char *all_report_room_info_xml = r_malloc(ROOM_INFO_XML_LEN * to_mc_report_live_channel_update_tcp.total_report_rooms);
	r_memset(all_report_room_info_xml, 0, ROOM_INFO_XML_LEN * to_mc_report_live_channel_update_tcp.total_report_rooms);
	cap_to_mc_report_room_info_xml(all_report_room_info_xml, to_mc_report_live_channel_update_tcp.to_mc_report_room_info, to_mc_report_live_channel_update_tcp.total_report_rooms);
	r_snprintf(xml, MAX_XML_LEN-1, IM_TO_MC_REPORT_LIVE_CHANNEL_UPDATE_XML_REQ_TCP,to_mc_report_live_channel_update_tcp.returncode,
			all_report_room_info_xml);
	r_free(all_report_room_info_xml);
	return 0;
}

int cap_to_mc_report_live_channel_update_xml_req_tcp_empty(char *xml, to_mc_report_live_channel_update_req_tcp_t to_mc_report_live_channel_update_tcp)
{
	r_snprintf(xml, MAX_XML_LEN -1, IM_TO_MC_REPORT_LIVE_CHANNEL_UPDATE_XML_REQ_TCP_EMPTY,
			to_mc_report_live_channel_update_tcp.returncode);
	return 0;
}



/*######################xml ½âÎö###########################*/

xmlNodePtr get_resp_return_code_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgHead");
	*pnode = get_children_node(tmp_node, BAD_CAST "ReturnCode");
	return *pnode;
}

xmlNodePtr get_resp_recserver_info_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgBody");
	tmp_node = get_children_node(tmp_node, BAD_CAST "GetRecServersInfoResp");
	*pnode = get_children_node(tmp_node, BAD_CAST "RecServerInfo");
	return *pnode;
}

xmlNodePtr get_resp_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr recserver_info_node)
{
	*pnode = get_children_node(recserver_info_node, BAD_CAST "RecServerIP");
	return *pnode;
}

xmlNodePtr get_resp_room_id_node(xmlNodePtr *pnode, xmlNodePtr recserver_info_node)
{
	*pnode = get_children_node(recserver_info_node, BAD_CAST "RoomID");
	return *pnode;
}

int get_recserver_info(recserver_info_t *recserver_info, xmlDocPtr pdoc, xmlNodePtr recservers_info_node)
{
#if 0
	xmlNodePtr recserver_ip_node = NULL;
	xmlNodePtr room_id_node = NULL;
	int total_rooms = 0;
	char room_id[8] = {0};
	get_resp_recserver_ip_node(&recserver_ip_node, recservers_info_node);
	get_current_node_value(recserver_info->recserver_ip, pdoc, recserver_ip_node);
	get_resp_room_id_node(&room_id_node, recservers_info_node);
	total_rooms = get_current_samename_node_nums(room_id_node);
	while(total_rooms) {
		get_current_node_value(room_id, pdoc, room_id_node);
		*(recserver_info->room_id_pool) = atoi(room_id);
		recserver_info->room_id_pool ++;
		room_id_node = room_id_node->next;
		total_rooms --;
	}
#endif	
	return 0;
}

int is_req_msg(xmlNodePtr  proot)
{
	if (xmlStrcmp (proot->name, REQ_ROOT_KEY) != 0)
	{
		return 0;
	}
	return 1;
}
int is_resp_msg(xmlNodePtr  proot)
{
	if (xmlStrcmp (proot->name, RESP_ROOT_KEY) != 0)
		{
				return 0;
		}
	return 1;
}

xmlNodePtr get_req_msg_code_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgHead");
	*pnode = get_children_node(tmp_node, BAD_CAST "MsgCode");
	return *pnode;
}

xmlNodePtr get_req_pass_key_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgHead");
	*pnode = get_children_node(tmp_node, BAD_CAST "PassKey");
	return *pnode;
}

xmlNodePtr get_req_setRecServerChannel_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgBody");
	tmp_node = get_children_node(tmp_node, BAD_CAST "SetRecServerChannelReq");
	*pnode = get_children_node(tmp_node, BAD_CAST "RecServerIP");
	return *pnode;
}

xmlNodePtr get_req_setRecServerChannel_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgBody");
	tmp_node = get_children_node(tmp_node, BAD_CAST "SetRecServerChannelReq");
	*pnode = get_children_node(tmp_node, BAD_CAST "RoomID");
	return *pnode;
}

xmlNodePtr get_req_getLiveChannel_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgBody");
	tmp_node = get_children_node(tmp_node, BAD_CAST "GetLiveChannelReq");
	*pnode = get_children_node(tmp_node, BAD_CAST "RecServerIP");
	return *pnode;
}

xmlNodePtr get_req_getLiveChannel_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgBody");
	tmp_node = get_children_node(tmp_node, BAD_CAST "GetLiveChannelReq");
	*pnode = get_children_node(tmp_node, BAD_CAST "RoomID");
	return *pnode;
}

xmlNodePtr get_req_stopUserLiveReq_recserver_ip_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgBody");
	tmp_node = get_children_node(tmp_node, BAD_CAST "StopUserLiveReq");
	*pnode = get_children_node(tmp_node, BAD_CAST "RecServerIP");
	return *pnode;
}

xmlNodePtr get_req_stopUserLiveReq_user_ip_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgBody");
	tmp_node = get_children_node(tmp_node, BAD_CAST "StopUserLiveReq");
	*pnode = get_children_node(tmp_node, BAD_CAST "UserIP");
	return *pnode;
}

xmlNodePtr get_req_stopUserLiveReq_room_id_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	xmlNodePtr  tmp_node = NULL;
	tmp_node = get_children_node(proot, BAD_CAST "MsgBody");
	tmp_node = get_children_node(tmp_node, BAD_CAST "StopUserLiveReq");
	*pnode = get_children_node(tmp_node, BAD_CAST "ChannelID");
	return *pnode;
}



