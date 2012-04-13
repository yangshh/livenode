#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include<sys/stat.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/un.h>
#include <semaphore.h>
#include <signal.h>

#include "reach_os.h"
#include "sys_info.h"
#include "user.h"
#include "mid_http.h"
#include "web_protocol_resolve.h"
#include "xml_base.h"
#include "xml_msg_management.h"
#include "connect_rec_server.h"
#include "media_msg.h"
#include "reach_socket.h"
#include "live_cfg.h"

#define		LIST_ENQ						(100)	        //并发最大监听数
#define		TCP_LIVE_CHANNELINFO_ALL	        "0.0.0.0"          // TCP信令2 获取全部通道信息
#define 		WEB_URL_HEAD			                 "http://"                        // web媒体中心地址
#define 		WEB_URL_BODY			                 "/HttpService.action"  // web媒体中心地址
#define 		WEB_URL_MAX_LEN	                          	100                                  //web媒体中心地址长度
#define 		WEB_URL_ERROR_SLEEP                           3				   //获取web媒体中心地址失败延迟时间

static web_global_vaiables_t g_web_global_vaiable;
extern connect_server_info_t g_connect_info[CHANNEL_CONNECT_MAX_NUM];
int8_t g_web_ip[IP_INFO_LEN] = {0};
// 全局直播结点网络占用率

int32_t get_web_url(char *ip)
{
	
	int32_t  url_head_len = r_strlen(WEB_URL_HEAD);
	int32_t  url_ip_len = r_strlen(g_web_ip);
	if(0 == url_ip_len) {
		printf("[%s] ---get ip failed !!!\n",__func__);
		return OPERATION_ERR;
	}
	int32_t  url_body_len =  r_strlen(WEB_URL_BODY);

	r_memcpy(ip,WEB_URL_HEAD,url_head_len);
	r_memcpy(&ip[url_head_len],g_web_ip,url_ip_len);
	r_memcpy(&ip[url_head_len+url_ip_len],WEB_URL_BODY,url_body_len);
	return OPERATION_SUCC;	
}

int32_t set_wet_url(char *ip,void *vag)
{
	int32_t					 p_ip_file =  0;
	int32_t 					write_file_len = 0;

	int32_t  					return_code = OPERATION_ERR;
	web_client_info_ex_t		*p_client_info 	= (web_client_info_ex_t *)vag;
	web_client_info_ex_t   		  c_client_info ;

	r_memcpy(&c_client_info, p_client_info, sizeof(web_client_info_ex_t));

	r_strcpy(ip,inet_ntoa(c_client_info.m_client_addr.sin_addr));
	printf("[%s]---temp_g_web_ip  is -----------------[%s]\n",__func__,ip);
	if(r_strcmp(g_web_ip ,ip) != 0)
	{
		r_strcpy(g_web_ip,ip);		
		p_ip_file = open(WEB_G_IP_MSG,O_WRONLY|O_CREAT);
		if(p_ip_file < 0)
		{
			printf("[%s]---open g_ip_file is error!\n",__func__);
		}
		else
		{
			write_file_len = write(p_ip_file,g_web_ip,IP_INFO_LEN);
			if(write_file_len == 0)
			{
				printf("[%s]---write ip_file is error!\n",__func__);
			}
			r_close(p_ip_file);
			return_code = OPERATION_SUCC;
		}						
		printf("[%s]---[g_web_ip] --- is [%s]\n",__func__,g_web_ip);
	}
	else
	{
		printf("[%s]---r_strcmp is NULL!\n",__func__);
		return_code = OPERATION_SUCC;
	}
	return return_code;
}

void judge_user_channel_http_report()
{

	int8_t  post_url[WEB_URL_MAX_LEN] = {0};
	r_memset(post_url,0,WEB_URL_MAX_LEN);
	if(get_web_url(post_url) == OPERATION_ERR)
	{
		printf("[%s]---[get_web_url] is error!\n",__func__);
		r_sleep(WEB_URL_ERROR_SLEEP);
		return ;
	}
		
	signal( SIGALRM, judge_user_channel_http_report);    
	
	if(web_post_live_user_info(post_url) != 0)
	{
		printf("[%s]---[web_post_live_user_info] is error !\n",__func__);
	}
	
	if(web_post_channel_update_info(post_url) != 0)
	{
		printf("[%s]---[web_post_channel_update_info] is error!\n",__func__);
	}
	alarm(HTTP_ALARM_TIME);
}


int32_t Traverse_gAmarry_free()
{
	int i_loop = 0;
	for( ; i_loop<CHANNEL_CONNECT_MAX_NUM ; i_loop ++)
	{
		if(g_connect_info[i_loop].m_connect_status == BC_CONNECT_CLOSE)
		{
			return i_loop;
		}		
	}
	if(i_loop == CHANNEL_CONNECT_MAX_NUM)
	{
		printf("[%s]---[CHANNEL_CONNECT_MAX_NUM] is error!\n",__func__);
		return OPERATION_ERR;
	}
	return OPERATION_ERR;
}

int32_t Traverse_gAmarry_usd(char *rec_server_ip,int32_t *room_amrry)
{
	int32_t i_loop = 0;
	int32_t j_loop = 0;
	for( ; i_loop<CHANNEL_CONNECT_MAX_NUM ; i_loop ++)
	{
		if(g_connect_info[i_loop].m_connect_status == BC_CONNECT_OK && strcmp(rec_server_ip,g_connect_info[i_loop].m_ip_addr) == 0)
		{
			room_amrry[j_loop] = i_loop;
			j_loop ++ ;
		}		
	}
	if(i_loop == CHANNEL_CONNECT_MAX_NUM && j_loop == 0)
	{
		printf("[%s] ---[CHANNEL_CONNECT_MAX_NUM] is error !\n",__func__);
		return OPERATION_ERR;
	}
	else
	{
		return j_loop ;
	}
}

int32_t msgcode_tcp_rec_serv_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id)
{
	int32_t					cmd_info_num = 0;
	int32_t 					ReturnCode = 0;
	web_cmd_parse_info_t		cmd_info;
	int32_t Room_ID_Amrry[ROOM_ID_MAX] ;
	int32_t Traverse_code = 0;
	int32_t temp_loop = 0;
	int32_t h_loop = 0;

	for(cmd_info_num = 0; cmd_info_num < ROOM_ID_MAX; cmd_info_num++)
	{	
		if(req_msg_room_id.rec_room_id[cmd_info_num] == -1)
		{
			r_memset(Room_ID_Amrry,-1,ROOM_ID_MAX);
			Traverse_code = 0;
			temp_loop = 0;
			Traverse_code = Traverse_gAmarry_usd(req_msg_room_id.rec_server_ip,Room_ID_Amrry);
			if(Traverse_code == -1)
			{
				printf("[%s]---there is no the server of IP !\n",__func__);
				ReturnCode = 0;
			}
			else
			{
				for(h_loop = 0;h_loop <Traverse_code; h_loop ++)
				{
					temp_loop = Room_ID_Amrry[h_loop];
					live_rec_server_exit(&g_connect_info[temp_loop]);
					printf("[%s]---rec_ip [%s]---rec_id[%d]---local_id[%d] is closed\n",__func__,g_connect_info[temp_loop].m_ip_addr,g_connect_info[temp_loop].m_channel,g_connect_info[temp_loop].m_id);
				}	
				ReturnCode = 1;
			}
		}
		else
		{
			r_memset(&cmd_info, 0, sizeof(web_cmd_parse_info_t));

			cmd_info.channel_id = Traverse_gAmarry_free();
			if(cmd_info.channel_id == OPERATION_ERR)
			{
				ReturnCode = 0;
				printf("[%s]---[Traverse_gAmarry_free]---no free !\n",__func__);
				printf("[%s]---recv_ip[%s]---recv_id[%d] is not connected\n",__func__,req_msg_room_id.rec_server_ip,req_msg_room_id.rec_room_id[cmd_info_num]);
				break;
			}
			
			cmd_info.rec_channel = req_msg_room_id.rec_room_id[cmd_info_num] ;
			r_memcpy(cmd_info.rec_ip_addr,req_msg_room_id.rec_server_ip,IP_INFO_LEN);

			live_rec_server_init(&g_connect_info[cmd_info.channel_id],&cmd_info);
			if(live_rec_server_connect(&g_connect_info[cmd_info.channel_id])!= 0)
			{
				printf("[%s]---[live_rec_server_connect] --- error!\n",__func__);
				ReturnCode = 0;
				break;
			}
			ReturnCode = 1;
		}	
	}					
	cap_return_code_xml_resp(&p_buffer[sizeof(int)],ReturnCode);
	return OPERATION_SUCC;
}

int32_t msgcode_tcp_live_channel_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id)
{
	int32_t j_loop_tcp  = 0; 
	int32_t h_loop_tcp = 0;
	to_mc_report_live_channel_update_req_tcp_t tcp_live_channel_info;
	r_memset(&tcp_live_channel_info , 0 , sizeof(to_mc_report_live_channel_update_req_tcp_t));
	// 查询直播结点全部通道信息
	if(r_strcmp(req_msg_room_id.rec_server_ip,TCP_LIVE_CHANNELINFO_ALL)  == 0)
	{
		for(j_loop_tcp = 0; j_loop_tcp < CHANNEL_CONNECT_MAX_NUM; j_loop_tcp ++)
		{
			if(g_connect_info[j_loop_tcp].m_id  != -1)
			{
				tcp_live_channel_info.total_report_rooms ++ ;
			}
		}
		if(tcp_live_channel_info.total_report_rooms == 0)
		{
			tcp_live_channel_info.returncode = 0;
		}
		else
		{
			tcp_live_channel_info.to_mc_report_room_info =(to_mc_report_room_info_t*)r_malloc(sizeof(to_mc_report_room_info_t)*tcp_live_channel_info.total_report_rooms);
			for(j_loop_tcp = 0; j_loop_tcp <CHANNEL_CONNECT_MAX_NUM;j_loop_tcp++)
			{
				if(g_connect_info[j_loop_tcp].m_id != -1)
				{
					r_memcpy(tcp_live_channel_info.to_mc_report_room_info[h_loop_tcp].rec_server_ip,g_connect_info[j_loop_tcp].m_ip_addr,IP_ADDR_LEN);
					tcp_live_channel_info.to_mc_report_room_info[h_loop_tcp].room_info.channel_id =g_connect_info[j_loop_tcp].m_id;
					tcp_live_channel_info.to_mc_report_room_info[h_loop_tcp].room_info.recserver_channel_id= g_connect_info[j_loop_tcp].m_channel;
					tcp_live_channel_info.to_mc_report_room_info[h_loop_tcp].room_info.recserver_connect_status =g_connect_info[j_loop_tcp].m_connect_status;
					h_loop_tcp ++ ;
					if(h_loop_tcp == tcp_live_channel_info.total_report_rooms)
					{
						break;
					}
				}
			}
		}
	}

	// 发送单一媒体中心所查询的通道信息	
	else
	{
		for(j_loop_tcp = 0 ;j_loop_tcp < CHANNEL_CONNECT_MAX_NUM ; j_loop_tcp ++)
		{				
			if(g_connect_info[j_loop_tcp].m_channel == req_msg_room_id.rec_room_id [0]&& r_strcmp(req_msg_room_id.rec_server_ip,g_connect_info[j_loop_tcp].m_ip_addr) == 0)
			{
				tcp_live_channel_info.to_mc_report_room_info =(to_mc_report_room_info_t*)r_malloc(sizeof(to_mc_report_room_info_t));
				tcp_live_channel_info.total_report_rooms = 1;
			
				r_memcpy(tcp_live_channel_info.to_mc_report_room_info->rec_server_ip,req_msg_room_id.rec_server_ip,IP_ADDR_LEN);
				tcp_live_channel_info.to_mc_report_room_info->room_info.channel_id =g_connect_info[j_loop_tcp].m_id;
				tcp_live_channel_info.to_mc_report_room_info->room_info.recserver_channel_id= req_msg_room_id.rec_room_id [0];
				tcp_live_channel_info.to_mc_report_room_info->room_info.recserver_connect_status =g_connect_info[j_loop_tcp].m_connect_status;
				tcp_live_channel_info.returncode = 1;
				break;
			}
		}
		if(tcp_live_channel_info.returncode == 0)
		{
			tcp_live_channel_info.returncode = 0;
		}
	}
	cap_to_mc_report_live_channel_update_xml_req_tcp_empty(&p_buffer[sizeof(int)],tcp_live_channel_info);
	return OPERATION_SUCC;
}

int32_t msgcode_tcp_live_users_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id)
{
	int8_t 					*temp_user_ips = NULL;
	int32_t 					ret_num = 0;
	int32_t 					ReturnCode = 0;
	live_users_info_resp_t         live_user_info;
	r_memset(&live_user_info,0,sizeof(live_users_info_resp_t));
	if(get_online_user_total() == 0)
	{
		printf("[%s]--- [get_online_user_total ]--- no users!\n",__func__);
		live_user_info.total_users = 0;
		live_user_info.ip_pool= NULL;
		get_live_user_info((void **)&temp_user_ips);
	}
	else
	{
		ret_num = get_live_user_info((void **)&temp_user_ips);
		if(ret_num ==-1)
		{
			printf("[%s]---[get_live_user_info ] ---the user_info is error---tcp!\n",__func__);
			ReturnCode= 0;
			cap_return_code_xml_resp(&p_buffer[sizeof(int)],ReturnCode);
			return OPERATION_SUCC;
		}
		else
		{
			live_user_info.total_users = get_online_user_total();
		 	live_user_info.ip_pool = temp_user_ips;
//			printf("[%s]--- [lives_ip online]  is  ---[%s]\n",__func__,live_user_info.ip_pool);
		}									
	}
	live_user_info.return_code= 1;
	live_user_info.total_bandwidth = 0;
	cap_live_users_info_xml_resp(&p_buffer[sizeof(int)], live_user_info);
	return OPERATION_SUCC;
}

int32_t msgcode_tcp_sys_info_func(int8_t *p_buffer,req_msg_serv_channel_t 	 req_msg_room_id)
{
	sys_info_t                                   cpu_sys_info;
	r_memset(&cpu_sys_info,0,sizeof(sys_info_t));

	cpu_sys_info.per_cpu = run_cpu();
	if(cpu_sys_info.per_cpu == -1)
	{
		printf("[%s]---[run_cpu]---run_cpu is error!\n",__func__);
		cpu_sys_info.return_code = 0;
		goto EXIT;
	}
	cpu_sys_info.per_ram = run_memory();
	if(cpu_sys_info.per_ram == -1)
	{
		printf("[%s]---[run_memory]---run_memory is error!\n",__func__);
		cpu_sys_info.return_code = 0;
		goto EXIT;
	}
	cpu_sys_info.per_network = run_throughput_boost();
	if(cpu_sys_info.per_network == -1)
	{
		printf("[%s]---[run_throughput_boost]---run_throughput_boost is error!\n",__func__);
		cpu_sys_info.return_code = 0;		
		goto EXIT;
	}
	cpu_sys_info.return_code = 1;
EXIT:
	cap_sys_info_xml_resp(&p_buffer[sizeof(int)] , cpu_sys_info);
	return OPERATION_SUCC;
}

int32_t package_web_cmd_http_ServInfoReq(int8_t *buffer)
{
	to_mc_msg_head_t req_head ;
	r_memset(&req_head, 0 ,sizeof(to_mc_msg_head_t));
	req_head.msg_code = MSGCODE_HTTP_REC_SERV;
	
	to_mc_get_recservers_info_req_t Servers_Info_Req ;		
	Servers_Info_Req.to_mc_msg_head = req_head;	
	cap_to_mc_get_recservers_info_xml_req (buffer,Servers_Info_Req);
	
	return OPERATION_SUCC;
}

int32_t package_web_cmd_http_LiveUpdateReq(int8_t *buffer)
{
	int8_t *temp_user_ips =NULL;
	int8_t *ifname = NULL;
	ifname = get_live_cfg_NetCard();
	to_mc_report_live_users_info_req_t report_msg_user_update;
	r_memset(&report_msg_user_update , 0 ,sizeof(to_mc_report_live_users_info_req_t));
	if(get_online_user_total() == 0)
	{
		printf("[%s] ---  no user!\n",__func__);
		report_msg_user_update.to_mc_msg_head.msg_code = MSGCODE_HTTP_LIVE_USERS;
		report_msg_user_update.report_live_users_info.total_users = 0;
		if(get_local_ip(ifname, report_msg_user_update.report_live_users_info.node_server_ip)== OPERATION_ERR)
		{
			printf("[%s] --- [get_local_ip] ---  error!\n",__func__);
			return -2;
		}
		report_msg_user_update.report_live_users_info.ip_pool = NULL;
		cap_to_mc_report_live_users_info_xml_req(buffer ,report_msg_user_update);
		return  0;
	}
	int32_t ret_num = get_live_user_info((void **)&temp_user_ips);
	if(ret_num ==-1)
	{
		printf("[%s] --- [get_live_user_info] --- error!\n", __func__);
		return -2;
	}
	else if(ret_num == 0)
	{
		printf("[%s] --- no users be changed !\n",__func__);
		return -1;
	}
	else
	{
		report_msg_user_update.to_mc_msg_head.msg_code = MSGCODE_HTTP_LIVE_USERS;
		report_msg_user_update.report_live_users_info.ip_pool = temp_user_ips;
		report_msg_user_update.report_live_users_info.total_bandwidth = 0 ;
		if(get_local_ip(ifname, report_msg_user_update.report_live_users_info.node_server_ip)== -1)
		{
			printf("[%s]---[fine_local_ip] ---error!\n",__func__);
			return  -2;
		}
		report_msg_user_update.report_live_users_info.total_users = ret_num;
	}
	cap_to_mc_report_live_users_info_xml_req(buffer ,report_msg_user_update);
	return 0;	
}

int32_t package_web_cmd_http_LiverChannelUpdateReq(int8_t *buffer)
{
	int32_t g_state_change = 0 ;
	int32_t j_loop = 0;
	int32_t h_loop = 0; 
	to_mc_report_live_channel_update_req_t report_msg_channel_update;
	r_memset(&report_msg_channel_update , 0 , sizeof(to_mc_report_live_channel_update_req_t));
	report_msg_channel_update.to_mc_msg_head.msg_code = MSGCODE_HTTP_LIVE_CHANNEL ;
	report_msg_channel_update.total_report_rooms = 0;
	for(j_loop = 0; j_loop < CHANNEL_CONNECT_MAX_NUM; j_loop ++)
	{
		if(g_connect_info[j_loop].m_id  != -1)
		{
			report_msg_channel_update.total_report_rooms ++;
		}
		if(g_connect_info[j_loop].m_state_change == 1 )
		{
			g_state_change = 1;
		}
	}
	if(report_msg_channel_update.total_report_rooms ==0 )
	{
		printf("[%s]---there is no channels!\n",__func__);
		return -1;
	}
	if(g_state_change == 0)
	{
		to_mc_get_recservers_info_req_t empty_xml_resq ;
		r_memset(&empty_xml_resq,0,sizeof(to_mc_get_recservers_info_req_t));
		empty_xml_resq.to_mc_msg_head = report_msg_channel_update.to_mc_msg_head;
		cap_to_mc_live_channel_empty_xml_resp(buffer,empty_xml_resq);
		return 0;
	}
	report_msg_channel_update.to_mc_report_room_info = (to_mc_report_room_info_t *)r_malloc(sizeof(to_mc_report_room_info_t)*report_msg_channel_update.total_report_rooms);
	for(j_loop = 0; j_loop < CHANNEL_CONNECT_MAX_NUM; j_loop++)
	{
		g_connect_info[j_loop].m_state_change = 0;
		if(g_connect_info[j_loop].m_id != -1)
		{
			r_memcpy(&(report_msg_channel_update.to_mc_report_room_info[h_loop ].rec_server_ip),&(g_connect_info[j_loop].m_ip_addr),IP_ADDR_LEN);
			report_msg_channel_update.to_mc_report_room_info[h_loop ].room_info.channel_id = g_connect_info[j_loop].m_id ;
			report_msg_channel_update.to_mc_report_room_info[h_loop ].room_info.recserver_connect_status = g_connect_info[j_loop].m_connect_status;
			report_msg_channel_update.to_mc_report_room_info[h_loop ].room_info.recserver_channel_id = g_connect_info[j_loop].m_channel;
			h_loop ++ ;
			if(h_loop == report_msg_channel_update.total_report_rooms)
			{
				break;
			}
		}		
	}	
	cap_to_mc_report_live_channel_update_xml_req(buffer,report_msg_channel_update);
	r_free(report_msg_channel_update.to_mc_report_room_info);
	return OPERATION_SUCC;
}

int32_t parse_web_cmd_http_ServInfoReq(int8_t *buffer)
{	
	recserver_info_t resp_msg_room_id ;
	web_cmd_parse_info_t	cmd_info;
	int8_t return_code_str[WEB_TEMP_MSG_LEN] = {0};	
	int8_t temp_channel_ID[WEB_TEMP_MSG_LEN] = {0}; 
	parse_xml_t *parse_xml_servinfo = (parse_xml_t *)r_malloc(sizeof(parse_xml_t));
	
	xmlNodePtr pnode_return_code = NULL ;
	xmlNodePtr pnode_servinfo_resp= NULL ;
	xmlNodePtr pnode_serv_ip = NULL;
	xmlNodePtr pnode_room_id = NULL;

	int32_t i_loop = 0;
	int32_t j_loop =0 ; 

	int32_t rets = OPERATION_ERR;

	r_memset(&cmd_info,0,sizeof(web_cmd_parse_info_t));
	cmd_info.rec_channel = -1;
	
	if(NULL == init_dom_tree(parse_xml_servinfo, buffer)) {
		printf("[%s]---init_dom_tree is error!\n",__func__);
		goto EXIT;
	}
	
	// 1.2.1 判断是否是xml 文件体		
	if(is_resp_msg(parse_xml_servinfo->proot) != 1)
	{
		printf("[%s]---xml_body is error!\n",__func__);
		goto EXIT;
	}	
	// 1.2.2 获取 ReturnCode 并做判断是否成功
	if(get_resp_return_code_node(&pnode_return_code,parse_xml_servinfo->proot) == NULL)
	{
		printf("[%s]---there is no broadcast infos!\n",__func__);
		rets = 0 ;
		goto EXIT;
	}
	if(get_current_node_value(return_code_str, WEB_TEMP_MSG_LEN, parse_xml_servinfo->pdoc,  pnode_return_code) != 0)
	{
		printf("[%s]---[ReturnCode]---[get_current_node_value] is error!\n",__func__);
		
		goto EXIT;
	} 
	
	if(return_code_str[0] != '1')
	{
		printf("[%s] ---[return_code is 0]\n",__func__);
		goto EXIT;
	}
	
	// 1.2.3 获取录播通道信息
	
	if(get_resp_recserver_info_node(&pnode_servinfo_resp,parse_xml_servinfo->proot) == NULL)
	{
		printf("[%s]---[get_resp_recserver_info_node] is error!\n",__func__);
		goto EXIT;
	}
	int32_t pnode_servinfo_node_num = get_current_samename_node_nums(pnode_servinfo_resp, parse_xml_servinfo->pdoc);
	if(pnode_servinfo_node_num == 0)
	{
		printf("[%s]---[pnode_servinfo_node_num] is NULL!\n",__func__);
		goto EXIT;
	}

	for(i_loop = 0;  i_loop <pnode_servinfo_node_num; i_loop++)
	{	
		r_memset(&resp_msg_room_id.recserver_ip , 0 ,sizeof(recserver_info_t));
		r_memset(resp_msg_room_id.room_id_pool,-1,ROOM_ID_MAX);	
		if(get_resp_recserver_ip_node(&pnode_serv_ip,pnode_servinfo_resp) == NULL )
		{		
			printf("[%s]---[get_resp_recserver_ip_node] is error!\n",__func__);
			goto EXIT;
		}
		
		if(get_current_node_value(resp_msg_room_id.recserver_ip, IP_ADDR_LEN,parse_xml_servinfo->pdoc,  pnode_serv_ip) != 0)
		{
			printf("[%s]---[recserver_ip]---[get_current_node_value] is error!\n",__func__);
			goto EXIT;
		}
		
		r_memcpy(cmd_info.rec_ip_addr,resp_msg_room_id.recserver_ip,IP_INFO_LEN);

		if(get_resp_room_id_node(&pnode_room_id,pnode_servinfo_resp) == NULL)
		{		
			printf("[%s]--- [get_resp_room_id_node] is error!\n",__func__);
			goto EXIT;
		}
		int32_t same_node_roomid_num = get_current_samename_node_nums(pnode_room_id, parse_xml_servinfo->pdoc);	
		if(same_node_roomid_num == 0)
		{
			printf("[%s]--- [same_node_roomid_num] is NULL!\n",__func__);
			goto EXIT;
		}	
		for(j_loop =0 ; j_loop <same_node_roomid_num; j_loop ++)
		{
			cmd_info.channel_id = Traverse_gAmarry_free();
			if(cmd_info.channel_id == OPERATION_ERR)
			{
				printf("[%s]--- there is no free space!\n",__func__);
				break;
			}
			
			if(get_current_node_value(temp_channel_ID,WEB_TEMP_MSG_LEN,parse_xml_servinfo->pdoc,pnode_room_id) == -1)
			{
				printf("[%s]---[temp_channel_ID]---[get_current_node_value] is error\n",__func__);
				goto EXIT;
			}

			cmd_info.rec_channel = atoi(temp_channel_ID);			
			if(cmd_info.rec_channel  == -1)
			{
				printf("[%s]---[cmd_info.rec_channel] is error!\n",__func__);
				goto EXIT;
			}

			printf("broadcast_channel_ID  [%d]+++broadcast_ip [%s]+++lives_node_channel_id  [%d]\n",cmd_info.rec_channel,cmd_info.rec_ip_addr,cmd_info.channel_id);
			
			r_memset(temp_channel_ID,0,4);
			pnode_room_id = pnode_room_id->next;
			
			live_rec_server_init(&g_connect_info[cmd_info.channel_id],&cmd_info);
			if(live_rec_server_connect(&g_connect_info[cmd_info.channel_id])!= 0)
			{
				printf("[%s] ---live_rec_server_connect is error!\n",__func__);
			}
		}		
		pnode_servinfo_resp = pnode_servinfo_resp->next;			
	}		

	rets = OPERATION_SUCC;
EXIT:	
	free(parse_xml_servinfo);
	parse_xml_servinfo = NULL;
	pnode_return_code = NULL ;
	pnode_servinfo_resp= NULL ;
	pnode_serv_ip = NULL;
	pnode_room_id = NULL;
	if(rets == OPERATION_SUCC)
	{
		printf("[%s]---htttp_1.1 is ok!\n",__func__);
	}
	else
	{
		printf("[%s]---htttp_1.1 is error!\n",__func__);
	}
	return rets;
}

int32_t parse_ResponseMsg(int8_t *buffer)
{
	int8_t return_code_str[WEB_TEMP_MSG_LEN] = {0};
	parse_xml_t *parse_xml_user = (parse_xml_t *)r_malloc(sizeof(parse_xml_t));
	xmlNodePtr pnode_return_code = NULL;
	int32_t return_code = OPERATION_ERR; 
	if(NULL == init_dom_tree(parse_xml_user, buffer)) {
		printf("[%s] ---[init_dom_tree] is error!\n",__func__);
		goto EXIT;
	}		
	if(is_resp_msg(parse_xml_user->proot) != 1)
	{	
		printf("[%s] ---[is_resp_msg] is error!\n",__func__);
		goto EXIT;
	}		
	if((xmlNodePtr)get_resp_return_code_node(&pnode_return_code,parse_xml_user->proot) == NULL)
	{
		printf("[%s] ---[get_resp_return_code_node] is error!\n",__func__);
		goto EXIT;
	}
	if(get_current_node_value(return_code_str,WEB_TEMP_MSG_LEN ,parse_xml_user->pdoc,  pnode_return_code) != 0)
	{
		printf("[%s] ---[get_current_node_value] is error!\n",__func__);
		goto EXIT;
	}
	if(return_code_str[0] == '0')
	{
		printf("[%s] ---[return_code_str] is NULL!\n",__func__);
		return_code  =  0; 
	}
	else
		return_code  =  1;
EXIT:
	if(parse_xml_user->pdoc != NULL)
		release_dom_tree(parse_xml_user->pdoc);
	r_free(parse_xml_user);
	parse_xml_user = NULL;
	pnode_return_code = NULL; 
	return return_code;
}

int32_t	parse_web_cmd_tcp(req_msg_serv_channel_t* req_msg_room_id,int32_t* req_msg_code, int8_t *buffer)
{	 
	
	parse_xml_t *parse_xml_tcp = (parse_xml_t *)r_malloc(sizeof(parse_xml_t));
	xmlNodePtr pnode_tcp =NULL;
	if(NULL == init_dom_tree(parse_xml_tcp,buffer)){
		printf("[%s] ---[init_dom_tree] is error!\n",__func__);
		goto EXIT;
	}
	int8_t temp_channel_ID[WEB_TEMP_MSG_LEN] = {0};

	int32_t return_code = OPERATION_ERR;
	int32_t i_loop = 0;
	
	req_msg_head_t req_msg_head;
	r_memset(&req_msg_head , 0,sizeof(req_msg_head_t));
	
	if(is_req_msg(parse_xml_tcp->proot) != 1)
	{
		printf("[%s] ---[is_req_msg] is error!\n",__func__);
		goto EXIT;
	}
	if(get_req_msg_code_node(&pnode_tcp, parse_xml_tcp->proot) == NULL)
	{
		printf("[%s] ---[get_req_msg_code_node] is error!\n",__func__);
		goto EXIT;
	}
	if(get_current_node_value(req_msg_head.req_code_tcp,REQ_CODE_TCP_SIZE, parse_xml_tcp->pdoc,  pnode_tcp) != 0)
	{
		printf("[%s] ---[req_code_tcp]---[get_current_node_value] is error!\n",__func__);
		goto EXIT;
	}
	if(get_req_pass_key_node(&pnode_tcp, parse_xml_tcp->proot)== NULL)
	{
		printf("[%s] ---[get_req_pass_key_node] is error!\n",__func__);
		goto EXIT;
	}
	if(get_current_node_value(req_msg_head.req_value_key,REQ_VALUE_KEY, parse_xml_tcp->pdoc,  pnode_tcp) != 0)
	{
		printf("[%s] ---[req_value_key]---[get_current_node_value] is error!\n",__func__);
		goto EXIT;
	}
	*req_msg_code = atoi(req_msg_head.req_code_tcp);
	if(*req_msg_code == -1)
	{
		printf("[%s] ---[atoi] is error!\n",__func__);
		goto EXIT;
	}
	printf("[%s] ---[req_code_tcp] is ++++++++++++++++[%d]!\n",__func__,*req_msg_code);
	switch(*req_msg_code)
	{
		case MSGCODE_TCP_REC_SERV:
		{
			if(get_req_setRecServerChannel_recserver_ip_node(&pnode_tcp,parse_xml_tcp->proot) == NULL)
			{
				printf("[%s] ---[MSGCODE_TCP_REC_SERV]---[get_req_setRecServerChannel_recserver_ip_node] is error!\n",__func__);
				goto EXIT;
			}
			if(get_current_node_value(req_msg_room_id->rec_server_ip,IP_ADDR_LEN, parse_xml_tcp->pdoc,  pnode_tcp) != 0)
			{
				printf("[%s] ---[MSGCODE_TCP_REC_SERV]---[rec_server_ip]---[get_current_node_value] is error!\n",__func__);
				goto EXIT;
			}
			if(get_req_setRecServerChannel_room_id_node(&pnode_tcp,parse_xml_tcp->proot) == NULL)
			{
				printf("[%s] ---[MSGCODE_TCP_REC_SERV]---[get_req_setRecServerChannel_room_id_node] is error!\n",__func__);
				goto EXIT;
			}
			int32_t same_node_num = get_current_samename_node_nums(pnode_tcp, parse_xml_tcp->pdoc);
			if(same_node_num == 0)
			{
				printf("[%s] ---[MSGCODE_TCP_REC_SERV]---[same_node_num] is NULL!\n",__func__);
				goto EXIT;
			}
			
			for( i_loop = 0;  i_loop<same_node_num; i_loop++)
			{
				r_memset(temp_channel_ID, 0,4);
				if(get_current_node_value(temp_channel_ID, WEB_TEMP_MSG_LEN,parse_xml_tcp->pdoc, pnode_tcp) != 0)
				{
					printf("[%s] ---[MSGCODE_TCP_REC_SERV]---[temp_channel_ID_1]---[get_current_node_value] is error!\n",__func__);
					goto EXIT;
				}				
				req_msg_room_id->rec_room_id[i_loop] = atoi(temp_channel_ID);
				printf("[%s]---[MSGCODE_TCP_REC_SERV]---rec_ip [%s]---rec_id [%d] from the Web!\n\n",__func__,req_msg_room_id->rec_server_ip,req_msg_room_id->rec_room_id[i_loop]);
				pnode_tcp = pnode_tcp->next;
			}
		}break;
		case MSGCODE_TCP_LIVE_CHANNEL:
		{
			r_memset(temp_channel_ID, 0,4);
			
			if(get_req_getLiveChannel_recserver_ip_node(&pnode_tcp,parse_xml_tcp->proot) == NULL)
			{
				printf("[%s] ---[MSGCODE_TCP_LIVE_CHANNEL]---[get_req_getLiveChannel_recserver_ip_node] is error!\n",__func__);
				goto EXIT;			
			}
			if(get_current_node_value(req_msg_room_id->rec_server_ip,IP_ADDR_LEN, parse_xml_tcp->pdoc,  pnode_tcp) != 0)
			{
				printf("[%s] ---[MSGCODE_TCP_LIVE_CHANNEL]---[rec_server_ip]---[get_current_node_value] is error!\n",__func__);
				goto EXIT;
			}
			if( get_req_getLiveChannel_room_id_node(&pnode_tcp,parse_xml_tcp->proot)== NULL)
			{
				printf("[%s] ---[MSGCODE_TCP_LIVE_CHANNEL]---[get_req_getLiveChannel_room_id_node] is error!\n",__func__);
				goto EXIT;
			}
			if(get_current_node_value(temp_channel_ID, WEB_TEMP_MSG_LEN,parse_xml_tcp->pdoc,  pnode_tcp) != 0)
			{

				printf("[%s] ---[MSGCODE_TCP_LIVE_CHANNEL]---[temp_channel_ID_2]---[get_current_node_value] is error!\n",__func__);
				goto EXIT;
			}
			req_msg_room_id->rec_room_id[0] =  atoi(temp_channel_ID);
			printf("[%s]---[MSGCODE_TCP_LIVE_CHANNEL]---rec_ip [%s]---rec_id [%d] from web!\n\n",__func__,req_msg_room_id->rec_server_ip,req_msg_room_id->rec_room_id[0]);
		}break;
		case MSGCODE_TCP_LIVE_USERS:
		{
			;
		}break;
		case MSGCODE_TCP_SYS_INFO:
		{
			;
		}break;
		case MSGCODE_TCP_STOP_LIVE:
		{
			int8_t temp_local_channelID [WEB_TEMP_MSG_LEN] = {0};
			if(get_req_stopUserLiveReq_recserver_ip_node(&pnode_tcp,parse_xml_tcp->proot) == NULL)
			{
				printf("[%s] ---[MSGCODE_TCP_STOP_LIVE]---[get_req_stopUserLiveReq_recserver_ip_node] is error!\n",__func__);
				goto EXIT;
			}
			if(get_current_node_value(req_msg_room_id->rec_server_ip, IP_ADDR_LEN,parse_xml_tcp->pdoc,  pnode_tcp) != 0)
			{
				printf("[%s] ---[MSGCODE_TCP_STOP_LIVE]---[rec_server_ip]---[get_current_node_value] is error!\n",__func__);
				goto EXIT;
			}
			if(get_req_stopUserLiveReq_user_ip_node(&pnode_tcp,parse_xml_tcp->proot) == NULL)
			{
				printf("[%s] ---[MSGCODE_TCP_STOP_LIVE]---[get_req_stopUserLiveReq_user_ip_node] is error!\n",__func__);
				goto EXIT;
			}
			if(get_current_node_value(req_msg_room_id->user_ip, IP_ADDR_LEN,parse_xml_tcp->pdoc,  pnode_tcp) != 0)
			{
				printf("[%s] ---[MSGCODE_TCP_STOP_LIVE]---[user_ip]---[get_current_node_value] is error!\n",__func__);
				goto EXIT;
			}
			if(get_req_stopUserLiveReq_room_id_node(&pnode_tcp,parse_xml_tcp->proot) == NULL)
			{
				printf("[%s] ---[MSGCODE_TCP_STOP_LIVE]---[get_req_stopUserLiveReq_room_id_node] is error!\n",__func__);
				goto EXIT;
			}
			if(get_current_node_value(temp_local_channelID,WEB_TEMP_MSG_LEN, parse_xml_tcp->pdoc,  pnode_tcp) != 0)
			{
				printf("[%s] ---[MSGCODE_TCP_STOP_LIVE]---[temp_local_channelID]---[get_current_node_value] is error!\n",__func__);
				goto EXIT;
			}
			req_msg_room_id->ChannelID = atoi(temp_local_channelID);
			printf("[%s]---[MSGCODE_TCP_STOP_LIVE]---rec_ip [%s]---user_ip [%s]---livenode_id [%d]\n\n",__func__,req_msg_room_id->rec_server_ip,req_msg_room_id->user_ip,req_msg_room_id->ChannelID);
		}break;
		case MSGCODE_TCP_REBOOT_PC:
		{
			;
		}break;
		default:
				break;
	}
	return_code = OPERATION_SUCC;
EXIT:
	if(parse_xml_tcp->pdoc != NULL)
		release_dom_tree(parse_xml_tcp->pdoc);
	r_free(parse_xml_tcp);
	parse_xml_tcp = NULL;
	return return_code;
}

int32_t web_post_live_user_info(int8_t *post_url)
{
	int8_t *post_recv_buffer = r_malloc(WEB_MSG_LEN);
	int8_t *post_to_buffer = r_malloc(WEB_MSG_LEN);
	int32_t post_recv_buffer_len = WEB_MSG_LEN;
	int32_t post_to_buf_len = 0;

	int32_t ResponseMsg_recode = 0 ;
	int32_t return_num_post = 0;
	int32_t recode_liveupdatereq = 0;

	int32_t return_code = OPERATION_ERR;
	
	r_memset(post_recv_buffer,0,WEB_MSG_LEN);
	r_memset(post_to_buffer,0,WEB_MSG_LEN);

	recode_liveupdatereq = package_web_cmd_http_LiveUpdateReq(post_to_buffer);

	if(recode_liveupdatereq == -1 )
	{
		printf("[%s]---[package_web_cmd_http_LiveUpdateReq] --- no users be changed !\n",__func__);
		return_code =  0;
		goto EXIT;
	}
	if(recode_liveupdatereq == -2)
	{
		printf("[%s]---[package_web_cmd_http_LiveUpdateReq] --- error!\n",__func__);
		goto EXIT;
	}
	post_to_buf_len = r_strlen(post_to_buffer)+1;

#if HTTP_SEND_P
	printf("[%s]---post_to_buffer 2.1 ------ \n",__func__);
	printf("%s\n",post_to_buffer);
#endif	
	return_num_post = mid_http_post(post_url, post_to_buffer, post_to_buf_len, post_recv_buffer, &post_recv_buffer_len);
	if(return_num_post != 0)
	{
		printf("[%s]---the return_num of mid_http_post is 2.1 --- [%d]\n",__func__,return_num_post);
		goto EXIT;
	}
#if HTTP_RECV_P
	printf("[%s]---post_recv_buffer is ------\n",__func__);
	printf("%s\n",post_recv_buffer);
#endif
	ResponseMsg_recode = parse_ResponseMsg(post_recv_buffer);
	if(ResponseMsg_recode == -1)
	{
		printf("[%s]---[parse_ResponseMsg] --- error!\n",__func__);
		goto EXIT;
	}
	return_code = OPERATION_SUCC;	
EXIT:
	r_free(post_recv_buffer);
	r_free(post_to_buffer);
	return return_code;
}

// http 上报录播服务器通道信息
int32_t web_post_channel_update_info(int8_t *post_url)
{
	int8_t *post_recv_buffer = r_malloc(WEB_MSG_LEN);
	int8_t *post_to_buffer = r_malloc(WEB_MSG_LEN);
	int32_t post_recv_buffer_len = WEB_MSG_LEN;
	int32_t post_to_buf_len = 0;

	int32_t ResponseMsg_recode = 0 ;
	int32_t return_num_post = 0;

	int32_t return_code = OPERATION_ERR;
	
	r_memset(post_recv_buffer,0,WEB_MSG_LEN);
	r_memset(post_to_buffer,0,WEB_MSG_LEN);

	if(package_web_cmd_http_LiverChannelUpdateReq(post_to_buffer) == -1)
	{
		printf("There is no server channels!\n");
		goto EXIT;
	}
	
	post_to_buf_len = r_strlen(post_to_buffer)+1;
#if HTTP_SEND_P
	printf("[%s]---post_to_buffer 2.1 ------ \n",__func__);
	printf("%s\n",post_to_buffer);
#endif
	return_num_post = mid_http_post(post_url, post_to_buffer, post_to_buf_len, post_recv_buffer, &post_recv_buffer_len);
	if(return_num_post != 0)
	{
		printf("[%s]---the return_num of mid_http_post is  3.1--- [%d]\n",__func__,return_num_post);
		goto EXIT;
	}

#if HTTP_RECV_P
	printf("[%s]---post_recv_buffer is ------\n",__func__);
	printf("%s\n",post_recv_buffer);
#endif

	ResponseMsg_recode = parse_ResponseMsg(post_recv_buffer);
	if(ResponseMsg_recode == -1)
	{
		printf("[%s]---parse_ResponseMsg is error!\n",__func__);
		goto EXIT;
	}
	return_code = OPERATION_SUCC;
EXIT:
	r_free(post_recv_buffer);
	r_free(post_to_buffer);
	return return_code;	
}

//TCP 协议20006 重启直播结点
static void restart_live(void)
{
	int8_t cmd[128] = {0};
	sprintf(cmd, "kill %d", getpid());
	system(cmd);
}

//结点接收媒体中心TCP 请求的线程函数
static void* create_listen_client_thread(void* arg)
{
	web_client_info_ex_t		*p_client_info 	= (web_client_info_ex_t *)arg;
	web_client_info_ex_t   		  c_client_info ;
	int8_t					*p_buffer		= NULL;
	int8_t 				          p_buffer_head[sizeof(int)] = {0};
	uint32_t 					*p_buffer_head_len = NULL;
	req_msg_serv_channel_t 	 req_msg_room_id;
	// 返回包
	int32_t 					ReturnCode = 0;
	int32_t   					send_buf_len =0 ;
	int32_t 					restart_live_flag = 0;

	int32_t					 reget_g_web_ip = 0;
	int8_t 					temp_g_web_ip[IP_INFO_LEN] = {0};
	
	int32_t 					req_code = -1;
	int32_t 					ret = 0;

	r_pthread_detach(r_pthread_self());
	p_buffer = (int8_t *)r_malloc(WEB_MSG_LEN);
	r_memset(p_buffer,0,WEB_MSG_LEN);

	r_memcpy(&c_client_info, p_client_info, sizeof(web_client_info_ex_t));
	
	sem_post(&g_web_global_vaiable.m_sem);

	while(1)
	{
		// 1.接受媒体中心TCP 请求(xml 格式包)
		r_memset(req_msg_room_id.rec_server_ip,0,IP_ADDR_LEN);
		r_memset(req_msg_room_id.user_ip,0,IP_ADDR_LEN);
		req_msg_room_id.ChannelID = -1;
		int32_t z_loop = 0;
		for(;z_loop<ROOM_ID_MAX;z_loop++)
		{
			req_msg_room_id.rec_room_id[z_loop] = -2;
		}	
		ret = recv_long_tcp_data_extend(c_client_info.m_client_socket,p_buffer_head,sizeof(int));
		if(ret != sizeof(int))
		{
			printf("[%s]---[recv_long_tcp_data_extend_head]---error --- [%d]\n",__func__,ret);
			goto EXIT;
		}
	  	p_buffer_head_len= (uint32_t *)p_buffer_head;
		if(*p_buffer_head_len > WEB_MSG_LEN)
		{
			printf("[%s]---[ WEB_MSG_LEN ] ---too small!\n",__func__);
			goto EXIT;
		}
		
		int32_t recv_len = *p_buffer_head_len ;
		ret = recv_long_tcp_data_extend(c_client_info.m_client_socket,p_buffer,recv_len);
		if(ret != recv_len)
		{
			printf("[%s]---[recv_long_tcp_data_extend_body]---error --- [%d]\n",__func__,ret);
			goto EXIT;
		}		
		
		printf("[%s]---tcp_buf_len [%d]---tcp_buf ---",__func__,ret);
		printf(" %s\n",p_buffer);

		// 2.  解析媒体中心TCP 请求(xml 格式包) 
		if(parse_web_cmd_tcp(&req_msg_room_id,&req_code,p_buffer) == -1)
		{
			printf("[%s]---[parse_web_cmd_tcp] --- error!\n",__func__);
			goto EXIT;
		}

		if(reget_g_web_ip == 0)
		{
			if(set_wet_url(temp_g_web_ip,&c_client_info) == 0)
			{
				reget_g_web_ip = 1;
			}
			else
			{
				printf("[%s]------is error!\n",__func__);
				goto EXIT;
			}
		}
		r_memset(p_buffer,0,WEB_MSG_LEN);

		// 3. 根据TCP 请求msgcode 进行相应处理
		switch(req_code)
		{
			case MSGCODE_TCP_REC_SERV:
			{
				msgcode_tcp_rec_serv_func(p_buffer,req_msg_room_id) ;

			}break;
			case MSGCODE_TCP_LIVE_CHANNEL:
			{
				msgcode_tcp_live_channel_func(p_buffer,req_msg_room_id);

			}break;			
			case MSGCODE_TCP_LIVE_USERS:
			{
				msgcode_tcp_live_users_func(p_buffer,req_msg_room_id);
	
			}break;
			case MSGCODE_TCP_SYS_INFO:
			{
				msgcode_tcp_sys_info_func(p_buffer,req_msg_room_id);
				
			}break;
			case MSGCODE_TCP_STOP_LIVE:
			{
				ReturnCode = del_live_user(req_msg_room_id.user_ip , req_msg_room_id.ChannelID);
				r_memset(p_buffer,0,WEB_MSG_LEN);
				cap_return_code_xml_resp(&p_buffer[sizeof(int)],ReturnCode);
			}break;
			case MSGCODE_TCP_REBOOT_PC:
			{
				ReturnCode = 1;
				r_memset(p_buffer,0,WEB_MSG_LEN);
				cap_return_code_xml_resp(&p_buffer[sizeof(int)],ReturnCode);
				restart_live_flag = 1;
				printf("[%s]---[restart_live_flag = 1]---lives_node reboort\n",__func__);
			}break;
			default:
				break;
		}	

		printf("[%s]---[TCP send to mc xml]  ---\n",__func__);
		printf("%s\n",&p_buffer[sizeof(int)]);
		printf("[%s]---[TCP send to mc the len of xml]---\n",__func__);
		printf("%d\n",send_buf_len);

		// 4. 直播结点根据处理情况做出相应回应发送应答给媒体中心
		send_buf_len = r_strlen(&p_buffer[sizeof(int)]);
		r_memcpy(p_buffer,&send_buf_len,sizeof(int));
		send_buf_len = send_buf_len +sizeof(int);	
		r_send(c_client_info.m_client_socket, p_buffer, send_buf_len, 0);
		printf("[%s]---TCP_MSG_CODE[%d]---sending.......\n",__func__,req_code);

		if(restart_live_flag == 1){
		goto EXIT;
		}
	}

EXIT:
	r_free(p_buffer);
	r_close(c_client_info.m_client_socket);
	if(restart_live_flag == 1) {
		printf("[%s] ---restart live !!!\n", __func__);
		restart_live();
	}
	return NULL;
}

//结点监听媒体中心tcp 连接的线程函数
static void* create_web_listen_thread(void* arg)
{
	int32_t						listen_socket	= 0;
	int32_t						client_socket	= 0;
	struct sockaddr_in				serv_addr;
	int32_t 						opt 			= 1;
	int32_t						len			= 0;
	
	pthread_t   					listen_client_thread;
	web_client_info_ex_t           		 client_info;

	sem_init(&g_web_global_vaiable.m_sem, 0, 0);

	// 1. 初始化网络环境
	listen_socket = r_socket(AF_INET, SOCK_STREAM, 0);

	if(listen_socket < 0)
	{
		printf("[%s]---[r_socket] is error!\n",__func__);
		goto EXIT;
	}
	r_setsockopt(listen_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)); 

	r_bzero( &serv_addr, sizeof(struct sockaddr_in));
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_port 			= htons(WEB_TCP_LISTEN_PORT);
	serv_addr.sin_addr.s_addr	= htonl(INADDR_ANY);

	 int on = 1;
         setsockopt( listen_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	
	if(bind(listen_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) < 0 )  
	{
		printf("[%s]---[bind] is error!\n",__func__);	
	  	goto EXIT;
	}
	if(listen(listen_socket, LIST_ENQ) < 0) 
	{
		printf("[%s]---[listen] is error!\n",__func__);
		goto EXIT;
	}
	while(1)
	{
		r_memset(&client_info,0,sizeof(web_client_info_ex_t));
		len = sizeof(struct sockaddr_in);	
		printf("[%s]--- beging to accept..............\n",__func__);
		// 2. 监听媒体中心请求连接
		client_socket = accept(listen_socket, (void*)&client_info.m_client_addr,(int32_t *)&len);
		if(client_socket < 0)
		{
			printf("[%s]---accept is error!--- [%d]\n",__func__,client_socket);
			fprintf(stderr,"Accept error:%s\n",strerror(errno));
  			switch(errno)
 			 {
    			case EPERM:
				printf("[%s]---EPERM\n",__func__);
			continue;
  			case ENOBUFS:
				printf("[%s]---ENOBUFS\n",__func__);
			continue;
  			default:
				printf("[%s]---others error!\n",__func__);
  			}
		}
		opt = 1;
		r_setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
		client_info.m_client_socket = client_socket;

		// 3. 启动TCP 请求处理线程函数
		if (r_pthread_create(&listen_client_thread, NULL, 
			create_listen_client_thread, &client_info)) 
		{
			printf("[%s]---Failed to create create_listen_client_thread = %s\n",__func__,strerror(errno));
			goto EXIT;
		}
		printf("[%s]---[r_pthread_create] is ok!\n",__func__);
		sem_wait(&g_web_global_vaiable.m_sem);
	}
	
EXIT:		
	sem_destroy(&g_web_global_vaiable.m_sem);
	return NULL;
}

//结点接收媒体中心http 请求的线程函数
static void* create_http_report_thread(void* arg)
{
	int8_t *post_recv_buffer = r_malloc(WEB_MSG_LEN);
	int8_t *post_to_buffer = r_malloc(WEB_MSG_LEN);
	int32_t post_recv_buffer_len = WEB_MSG_LEN;
	int32_t post_to_buf_len = 0;
	
	int32_t return_num_post = 0;
	int8_t  post_url[WEB_URL_MAX_LEN] = {0};

	if(post_recv_buffer == NULL ||post_to_buffer == NULL)
	{
		printf("[%s]---[create_http_report_thread] malloc ------is error!\n",__func__);
		goto EXIT;
	}

	// 1. 通过内存或是文件获取媒体中心地址
	int32_t p_ip_file =  0;
	p_ip_file = open(WEB_G_IP_MSG,O_RDONLY);
	if(p_ip_file < 0)
	{
		printf("[%s]---open g_ip_file is error!\n",__func__);
	}
	else
	{
		read(p_ip_file,g_web_ip,IP_INFO_LEN);
		printf("[%s]---g_web_ip from ip_file is ---[%s]\n",__func__,g_web_ip);
		r_close(p_ip_file);
	}	
	// *以下代码用于隔离媒体中心交互模块，直连录播服务器
#if ONLY_REC
	
	web_cmd_parse_info_t	cmd_info;
	r_memcpy(cmd_info.rec_ip_addr,REC_IP,15);
	printf("[%s]---[%s]\n",__func__, REC_IP);
	cmd_info.channel_id = 0;
	cmd_info.rec_channel = 0;
	
	live_rec_server_init(&g_connect_info[cmd_info.channel_id],&cmd_info);
	if(live_rec_server_connect(&g_connect_info[cmd_info.channel_id])!= 0)
	{
		printf("[%s]---live_rec_server_connect is error!\n",__func__);
	}

	cmd_info.channel_id = 1;
	cmd_info.rec_channel = 1;
	
	live_rec_server_init(&g_connect_info[cmd_info.channel_id],&cmd_info);
	if(live_rec_server_connect(&g_connect_info[cmd_info.channel_id])!= 0)
	{
		printf("[%s]---live_rec_server_connect is error!\n",__func__);
	}
	printf("*****---------------[%s]-----------------******\n",__func__);
	goto EXIT;

#endif
	// 2. HTTP 协议1 结点向媒体中心请求获取注册录播信息并处理
	if(package_web_cmd_http_ServInfoReq(post_to_buffer) == -1)
	{
		printf("[%s]---[package_web_cmd_http_ServInfoReq] is error!\n",__func__);
		goto EXIT;
	}
	post_to_buf_len = r_strlen(post_to_buffer)+1;
#if HTTP_SEND_P
	printf("[%s]---post_to_buffer 1.1 ---- \n",__func__);
	printf("%s\n",post_to_buffer);
#endif
	while(1)
	{
		r_memset(post_url,0,WEB_URL_MAX_LEN);
		if(get_web_url(post_url) == OPERATION_ERR)
		{
			r_sleep(WEB_URL_ERROR_SLEEP);
			continue;
		}
		printf("[%s]---[web_1.1 url ] ------- [%s]\n",__func__,post_url);
		r_memset(post_recv_buffer,0,WEB_MSG_LEN);
		return_num_post = mid_http_post(post_url, post_to_buffer, post_to_buf_len, post_recv_buffer, &post_recv_buffer_len);
		if(return_num_post == -1)
		{
			printf("[%s]---[mid_http_post] is error!---the return_num_post is [%d]\n",__func__,return_num_post);
			r_sleep(HTTP_ALARM_TIME);
		}
		else
		{	
			printf("[%s]---[mid_http_post] is ok!\n",__func__);
			break;
		}
	}
#if HTTP_RECV_P
	printf("[%s]---recv buf_1.3 is ----\n",__func__);
	printf("%s\n",post_recv_buffer);
#endif

	// 3. 直播结点循环上报HTTP 协议2、3 在线用户数以及通道信息
	if(parse_web_cmd_http_ServInfoReq(post_recv_buffer) == -1)
	{
		printf("[%s]---parse_web_cmd_http_servinfo is error!\n",__func__);
		goto EXIT;
	}
	signal( SIGALRM, judge_user_channel_http_report);    
    	signal(SIGCLD, SIG_IGN); 
		
    	kill(getpid(), SIGALRM); 

    	while(1) 
    	{ 
        		pause(); 
    	}  	
  	
EXIT:
	printf("[%s]---there is a error in create_http_report_thread !\n",__func__);
	r_free(post_recv_buffer);
	r_free(post_to_buffer);
	return NULL;
	
}


//初始化直播结点与媒体中心交互模块
int32_t	init_web_protoclo_resolve_func(void)
{
	pthread_t   p_web_listen_thread;
	pthread_t   p_http_report_thread;
	// 读配置文件
	g_connect_info_init();
	
	if (r_pthread_create(&p_web_listen_thread, NULL, 
		create_web_listen_thread, NULL)) 
	{
		printf("[%s]---Failed to create connect thread thread = %s\n",__func__,strerror(errno));
		return CREATE_THREAD_ERR;	
	}

	if (r_pthread_create(&p_http_report_thread, NULL, 
		create_http_report_thread, NULL)) 
	{
		printf("[%s]---Failed to create_http_report_thread = %s\n",__func__,strerror(errno));
		return CREATE_THREAD_ERR;	
	}

	
	return OPERATION_SUCC;
}




