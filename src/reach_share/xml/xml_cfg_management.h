#ifndef	__XML_CFG__
#define	__XML_CFG__

#include "xml_base.h"
// 直播结点配置文件相关处理函数
int is_live_cfg(xmlNodePtr  proot);
xmlNodePtr get_cfg_info_UserLimit_node(xmlNodePtr *pnode, xmlNodePtr proot);
xmlNodePtr get_cfg_info_NetCard_node(xmlNodePtr *pnode, xmlNodePtr proot);
xmlNodePtr get_cfg_info_NetBandwidthLimit_node(xmlNodePtr *pnode, xmlNodePtr proot);

#endif

