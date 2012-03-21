
#include "xml_base.h"

void init_dom_tree(parse_xml_t *parse_xml, const char *xml)
{
	parse_xml->pdoc = xmlParseMemory(xml , strlen(xml));
	// 获取 xml 文档对象的根节对象
	parse_xml->proot = xmlDocGetRootElement (parse_xml->pdoc);
}

void release_dom_tree(xmlDocPtr pdoc)
{
	/* 关闭和清理 */
	xmlFreeDoc (pdoc);
	xmlCleanupParser ();
}

xmlNodePtr get_children_node(xmlNodePtr curNode, const xmlChar * key)
{ 
	xmlNodePtr pcur = curNode->xmlChildrenNode; 
	while (pcur != NULL) { 
		if ((!xmlStrcmp(pcur->name, key))) { 
			return pcur;
		} 
		pcur = pcur->next; 
	} 
	return NULL; 
} 

int get_current_samename_node_nums(xmlNodePtr curNode)
{
	int i = 0;
	while(NULL != curNode) {
		i ++;
		curNode = curNode->next;
	}
	return i;
}

int32_t get_current_node_value(char *value, int32_t value_len, xmlDocPtr pdoc,  xmlNodePtr curNode)
{
	xmlChar *key = NULL; 
	key = xmlNodeListGetString(pdoc, curNode->xmlChildrenNode, 1);
	if(NULL == key) {
		return -1;
	}
	if(xmlStrlen(key) > value_len ||xmlStrlen(key) == value_len)
	{
		return -1;
	}
	
	r_memcpy(value, key, xmlStrlen(key));
	xmlFree(key);
	return 0;
}

