
#ifndef __XML_BASE__
#define __XML_BASE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reach_os.h"

#include "parser.h"
#include "tree.h"
/**
*@brief dom树的信息
*/
typedef struct parse_xmls {
	/** @dom树句柄*/
	xmlDocPtr pdoc;	
	/** @dom树的根节点*/
	xmlNodePtr  proot;	
} parse_xml_t;

/**
* @brief 将xml格式字符串生成dom树及获取其根节点
* @param  [OUT] parse_xml 生成dom树的信息
* @param  [IN] xml	xml字符串
* @exception  none  
* @return  none
* @note  none
* @remarks  none
*/
void init_dom_tree(parse_xml_t *parse_xml, const char *xml);

/**
* @brief 释放清理dom树
* @param  [OUT] pdoc dom树句柄
* @exception  none
* @return  none
* @note  none
* @remarks  none
*/
void release_dom_tree(xmlDocPtr pdoc);

/**
* @brief 获取子节点
* @param  [IN] curNode 当前节点
* @param  [IN] 子节点名
* @exception  none  
* @return  子节点
* @note  none
* @remarks  none
*/
xmlNodePtr get_children_node(xmlNodePtr curNode, const xmlChar * key);

/**
* @brief 获取当前节点的值
* @param  [OUT] value 存储获取的节点的值buffer
* @param  [OUT] pdoc dom树句柄
* @param  [IN] curNode 当前要获取值的节点
* @exception  none  
* @return  成功返回0 ，失败返回-1
* @note  none
* @remarks  none
*/
int32_t get_current_node_value(char *value, int32_t value_len, xmlDocPtr pdoc,  xmlNodePtr 
curNode);

/**
* @brief 获取当前同名节点个数
* @param  [IN]  curNode 当前节点
* @exception  none
* @return  成功返回当前同名节点个数
* @note  none
* @remarks  none
*/
int get_current_samename_node_nums(xmlNodePtr curNode);

#endif //__XML_BASE__

