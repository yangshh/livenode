
#include "live_cfg.h"
#include "xml_base.h"
#include "xml_cfg_management.h"
#include "stdint.h"
#define LIVE_CFG_FILE_PATH "/usr/local/reach/LiveCfg.xml"

#define	LIVE_CFG_MAX_LEN 			1024
#define	LIVE_CFG_NETCARD_MAX_LEN	16
#define	LIVE_CFG_INFO_LEN			64
#define	LIVE_CFG_VALUE_LEN			16

typedef struct live_cfg_infos {
	int32_t m_effective;
	int32_t m_UserLimit ;
	int8_t   m_NetCard[LIVE_CFG_NETCARD_MAX_LEN];
	int32_t m_NetBandwidthLimit;
} live_cfg_info_t;

live_cfg_info_t g_live_cfg_infos ;

void init_g_live_cfg_info(void)
{
	g_live_cfg_infos.m_effective = -1;
	g_live_cfg_infos.m_NetBandwidthLimit = -1;
	g_live_cfg_infos.m_UserLimit = -1;
	r_memset(g_live_cfg_infos.m_NetCard , 0, LIVE_CFG_NETCARD_MAX_LEN);
}

int32_t get_live_cfg_infos(int8_t *cfg_buf)
{
	int32_t write_file_len = 0;
	int32_t p_cfg_file = 0;
	int32_t return_code = -1;
	p_cfg_file = open(LIVE_CFG_FILE_PATH, O_RDONLY);

	if(p_cfg_file < 0) {
		printf("[%s]---open p_cfg_file is error!\n", __func__);
	} 
	
	else
	{
		write_file_len = read(p_cfg_file, cfg_buf, LIVE_CFG_MAX_LEN);
		if(write_file_len == 0) {
			printf("[%s]---read p_cfg_file is error!\n", __func__);
			
		}
		else{
			return_code = 0;
		}
		r_close(p_cfg_file);		
	}

	return return_code;
}


int32_t parse_live_cfg_infos(int8_t *cfg_buf)
{
	parse_xml_t *parse_xml_cfg_infos = (parse_xml_t *)r_malloc(sizeof(parse_xml_t));
	xmlNodePtr temp_pnode = NULL;
	int32_t return_code = -1;
	int8_t temp_cfg_info_buf[LIVE_CFG_INFO_LEN] = {0};

	if(NULL == init_dom_tree(parse_xml_cfg_infos, cfg_buf))
	{
		printf("[%s] ---[init_dom_tree] is error!\n", __func__);
		goto EXIT;
	}
	
	if(is_live_cfg(parse_xml_cfg_infos->proot) != 1) {
		printf("[%s] ---[is_live_cfg] is error!\n", __func__);
		goto EXIT;
	}

	if(get_cfg_info_UserLimit_node(&temp_pnode, parse_xml_cfg_infos->proot) == NULL) {
		printf("[%s] ---[get_cfg_info_UserLimit_node] is error!\n", __func__);
		goto EXIT;
	}

	if(get_current_node_value(temp_cfg_info_buf, LIVE_CFG_VALUE_LEN , parse_xml_cfg_infos->pdoc,  temp_pnode) != 0) {
		printf("[%s] ---[get_current_node_value] is error!\n", __func__);
		goto EXIT;
	}

	g_live_cfg_infos.m_UserLimit	= atoi(temp_cfg_info_buf);
	r_memset(temp_cfg_info_buf, 0, LIVE_CFG_INFO_LEN);

	if(get_cfg_info_NetCard_node(&temp_pnode, parse_xml_cfg_infos->proot) == NULL) {
		printf("[%s] ---[get_cfg_info_NetCard_node] is error!\n", __func__);
		goto EXIT;
	}

	if(get_current_node_value(temp_cfg_info_buf, LIVE_CFG_VALUE_LEN , parse_xml_cfg_infos->pdoc,  temp_pnode) != 0) {
		printf("[%s] ---[get_current_node_value] is error!\n", __func__);
		goto EXIT;
	}

	if((r_strlen(temp_cfg_info_buf) + 1) > LIVE_CFG_NETCARD_MAX_LEN) {
		printf("[%s]---[the net_card is too long]!\n", __func__);
		goto EXIT;
	}

	r_memcpy(g_live_cfg_infos.m_NetCard, temp_cfg_info_buf, LIVE_CFG_NETCARD_MAX_LEN);
	r_memset(temp_cfg_info_buf, 0, LIVE_CFG_INFO_LEN);

	if(get_cfg_info_NetBandwidthLimit_node(&temp_pnode, parse_xml_cfg_infos->proot) == NULL) {
		printf("[%s] ---[get_cfg_info_NetBandwidthLimit_node] is error!\n", __func__);
		goto EXIT;
	}

	if(get_current_node_value(temp_cfg_info_buf, LIVE_CFG_VALUE_LEN , parse_xml_cfg_infos->pdoc,  temp_pnode) != 0) {
		printf("[%s] ---[get_current_node_value] is error!\n", __func__);
		goto EXIT;
	}

	g_live_cfg_infos.m_NetBandwidthLimit = atoi(temp_cfg_info_buf);
	g_live_cfg_infos.m_effective = 0;
	return_code = 0;
EXIT:

	if(parse_xml_cfg_infos->pdoc != NULL) {
		release_dom_tree(parse_xml_cfg_infos->pdoc);
	}

	r_free(parse_xml_cfg_infos);
	temp_pnode = NULL;
	return return_code;
}

int32_t init_live_cfg_env(void)
{
	int8_t live_cfg_buf[LIVE_CFG_MAX_LEN] = {0};
	int32_t return_code = -1;

	init_g_live_cfg_info();

	if(get_live_cfg_infos(live_cfg_buf) == -1) {
		printf("[%s]---[get_live_cgf_infos]--- is error!\n", __func__);
		goto EXIT;
	}

	printf("[%s]---[get_live_cgf_infos] is ----\n", __func__);
	printf("%s\n", live_cfg_buf);

	if(parse_live_cfg_infos(live_cfg_buf) == -1) {
		printf("[%s]---[parse_live_cfg_infos]--- is error!\n", __func__);
		goto EXIT;
	}

	return_code = 0;
EXIT:
	return return_code;
}

int32_t get_live_cfg_UserLimit(void)
{
	if(g_live_cfg_infos.m_effective == 0) {
		return g_live_cfg_infos.m_UserLimit;
	}

	return -1;
}
int8_t *get_live_cfg_NetCard(void)
{
	if(g_live_cfg_infos.m_effective == 0) {
		return (int8_t *)(g_live_cfg_infos.m_NetCard);
	}

	return NULL;
}
int32_t get_live_cfg_NetBandwidthLimit(void)
{
	if(g_live_cfg_infos.m_effective == 0) {
		return g_live_cfg_infos.m_NetBandwidthLimit;
	}

	return -1;
}


