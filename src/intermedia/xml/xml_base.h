
#ifndef __XML_BASE__
#define __XML_BASE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reach_os.h"

#include "parser.h"
#include "tree.h"
/**
*@brief dom������Ϣ
*/
typedef struct parse_xmls {
	/** @dom�����*/
	xmlDocPtr pdoc;	
	/** @dom���ĸ��ڵ�*/
	xmlNodePtr  proot;	
} parse_xml_t;

/**
* @brief ��xml��ʽ�ַ�������dom������ȡ����ڵ�
* @param  [OUT] parse_xml ����dom������Ϣ
* @param  [IN] xml	xml�ַ���
* @exception  none  
* @return  none
* @note  none
* @remarks  none
*/
void init_dom_tree(parse_xml_t *parse_xml, const char *xml);

/**
* @brief �ͷ�����dom��
* @param  [OUT] pdoc dom�����
* @exception  none
* @return  none
* @note  none
* @remarks  none
*/
void release_dom_tree(xmlDocPtr pdoc);

/**
* @brief ��ȡ�ӽڵ�
* @param  [IN] curNode ��ǰ�ڵ�
* @param  [IN] �ӽڵ���
* @exception  none  
* @return  �ӽڵ�
* @note  none
* @remarks  none
*/
xmlNodePtr get_children_node(xmlNodePtr curNode, const xmlChar * key);

/**
* @brief ��ȡ��ǰ�ڵ��ֵ
* @param  [OUT] value �洢��ȡ�Ľڵ��ֵbuffer
* @param  [OUT] pdoc dom�����
* @param  [IN] curNode ��ǰҪ��ȡֵ�Ľڵ�
* @exception  none  
* @return  �ɹ�����0 ��ʧ�ܷ���-1
* @note  none
* @remarks  none
*/
int32_t get_current_node_value(char *value, int32_t value_len, xmlDocPtr pdoc,  xmlNodePtr 
curNode);

/**
* @brief ��ȡ��ǰͬ���ڵ����
* @param  [IN]  curNode ��ǰ�ڵ�
* @exception  none
* @return  �ɹ����ص�ǰͬ���ڵ����
* @note  none
* @remarks  none
*/
int get_current_samename_node_nums(xmlNodePtr curNode);

#endif //__XML_BASE__

