
#include "xml_base.h"
void *init_dom_tree(parse_xml_t *parse_xml, const char *xml)
{
	parse_xml->pdoc = xmlParseMemory(xml , strlen(xml));
	if(NULL == parse_xml->pdoc) {
		return NULL;
	}
	// 获取 xml 文档对象的根节对象
	parse_xml->proot = xmlDocGetRootElement(parse_xml->pdoc);
	if(NULL == parse_xml->proot) {
		return NULL;
	}
	return parse_xml;
}

void *init_file_dom_tree(parse_xml_t *parse_xml, const char *file_name)
{
	//parse_xml->pdoc = xmlParseFile(file_name); //段错误
	parse_xml->pdoc = xmlReadFile(file_name, "UTF-8", XML_PARSE_NOBLANKS | XML_PARSE_NODICT);
	if(NULL == parse_xml->pdoc) {
		return NULL;
	}
	// 获取 xml 文档对象的根节对象
	parse_xml->proot = xmlDocGetRootElement(parse_xml->pdoc);
	if(NULL == parse_xml->proot) {
		return NULL;
	}
	return parse_xml;
}

void release_dom_tree(xmlDocPtr pdoc)
{
	/* 关闭和清理 */
	xmlFreeDoc(pdoc);
	xmlCleanupParser();
}

xmlNodePtr get_children_node(xmlNodePtr curNode, const xmlChar *key)
{
	xmlNodePtr pcur = curNode->xmlChildrenNode;

	while(pcur != NULL) {
		if(!xmlStrcmp(pcur->name, key)) {
			return pcur;
		}

		pcur = pcur->next;
	}

	return NULL;
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


int get_current_samename_node_nums(xmlNodePtr curNode)
{
	int i = 0;
	xmlChar *key = NULL;
	key =  curNode->name;

	while(NULL != curNode) {
		if(!xmlStrcmp(curNode->name, key))  {
			i ++;
		}
		curNode = curNode->next;
	}

	return i;
}

xmlNodePtr add_new_node(xmlNodePtr parent, const xmlChar *name, const xmlChar *content)
{
	xmlNodePtr pcur;
	pcur = xmlNewTextChild(parent, NULL, name, content);
	return pcur;
}

int	modify_node_value(xmlNodePtr cur, const xmlChar *content)
{
	xmlNodeSetContentLen(cur, content, xmlStrlen(content));
	return 0;
}

xmlNodePtr del_cur_node(xmlNodePtr pnode)
{
	xmlNodePtr tempNode;
	tempNode = pnode->next;
	xmlUnlinkNode(pnode);
	xmlFreeNode(pnode);
	pnode = tempNode;

	return pnode;
}

xmlAttrPtr xml_prop_set(xmlNodePtr node, const xmlChar *name, const xmlChar *value)
{
	xmlAttrPtr ptr;
	ptr = xmlSetProp(node, name, value);
	return ptr;
}

int xml_prop_del(xmlNodePtr curNode, const xmlChar *attr_name)
{
	if(xmlHasProp(curNode, attr_name)) {
		xmlAttrPtr attrPtr = curNode->properties;

		while(attrPtr != NULL) {
			if(!xmlStrcmp(attrPtr->name, attr_name)) {
				xmlAttrPtr tmp = attrPtr->next;
				xmlRemoveProp(attrPtr);
				attrPtr = tmp;
				continue;
			}

			attrPtr = attrPtr->next;
		}
	}
	return 0;
}

xmlNodePtr xml_add_new_child(xmlNodePtr parent, xmlNsPtr ns,
                             const xmlChar *name, const xmlChar *content)
{
	xmlNodePtr node;
	node = xmlNewTextChild(parent, ns, name, content);
	return node;
}

int xml_node_value_set(xmlNodePtr cur, const xmlChar *content, int len)
{
	xmlNodeSetContentLen(cur, content, len);
	return 0;
}

