
#include "xml_cfg_management.h"

#define CFG_ROOT_KEY (BAD_CAST "LiveCfg")
int is_live_cfg(xmlNodePtr  proot)
{
	if(xmlStrcmp(proot->name, CFG_ROOT_KEY) != 0) {
		return 0;
	}

	return 1;
}

xmlNodePtr get_cfg_info_UserLimit_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	*pnode = get_children_node(proot, BAD_CAST "UserLimit");
	return *pnode;
}

xmlNodePtr get_cfg_info_NetCard_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	*pnode = get_children_node(proot, BAD_CAST "NetCard");
	return *pnode;
}

xmlNodePtr get_cfg_info_NetBandwidthLimit_node(xmlNodePtr *pnode, xmlNodePtr proot)
{
	*pnode = get_children_node(proot, BAD_CAST "NetBandwidthLimit");
	return *pnode;
}



