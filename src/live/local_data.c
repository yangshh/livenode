
#include "local_data.h"

lds_t *get_local_data_src_handle(int index)
{
	if(MAX_CH < index || 0 > index) {
		return NULL;
	}
	return &(lds[index]);
}

int32_t get_list_len(int32_t index)
{
	lds_t *s = get_local_data_src_handle(index);
	if(NULL == s) {
		return -1;
	}
	return s->list_len;
}

int32_t set_list_len(int32_t index, int32_t list_len)
{
	lds_t *s = get_local_data_src_handle(index);
	if(NULL == s) {
		return -1;
	}
	s->list_len = list_len;
	return list_len;
}

int32_t get_channel_id(int32_t index)
{
	lds_t *s = get_local_data_src_handle(index);
	if(NULL == s) {
		return -1;
	}
	return s->chid;
}

int32_t set_channel_id(int32_t index, int32_t id)
{
	lds_t *s = get_local_data_src_handle(index);
	if(NULL == s) {
		return -1;
	}
	s->chid = id;
	return id;
}

node_t *get_list_head(int32_t index)
{
	lds_t *s = get_local_data_src_handle(index);
	if(NULL == s) {
		return NULL;
	}
	return s->head;
}

node_t *set_list_head(int32_t index, node_t *head)
{
	lds_t *s = get_local_data_src_handle(index);
	if(NULL == s) {
		return NULL;
	}
	s->head = head;
	return head;
}

void init_local_data_src(void)
{
	int i;
	lds_t *s = NULL;
	for(i = 0; i < MAX_CH; i++) {
		s = get_local_data_src_handle(i);
		s->chid = -1;
		s->head = NULL;
		s->list_len = 0;
	}
}

int32_t get_local_data_index(int32_t chid)
{
	int i;
	lds_t *s = NULL;
	/** 到表里查找通道*/
	for(i = 0; i < MAX_CH; i++) {
		s = get_local_data_src_handle(i);
		if(s->chid == chid) {
			//printf("[find_list_index] old[%d]!!!\n", chid);
			return i;
		}
	}
	/** 没有该通道建立新通道*/
	for(i = 0; i < MAX_CH; i ++) {
		s = get_local_data_src_handle(i);
		if(s->chid == -1) {
			s->chid = chid;
			//printf("[find_list_index] new[%d]!!!\n", chid);
			return i;
		}
	}
	return -1;
}

node_t *find_list_head(uint16_t chid)
{
	int i;
	lds_t *s = NULL;
	/** 到表里查找通道*/
	for(i = 0; i < MAX_CH; i++) {
		s = get_local_data_src_handle(i);
		if(s->chid == chid) {
			return s->head;
		}
	}
	return NULL;
}

int32_t find_list_node(node_t *n, uint16_t chid)
{
	node_t *head;
	head = find_list_head(chid);
	if(NULL == head) {
		return 0;
	}
	while(head) {
		if(n == head) {
			return 1;
		}
		head = head->next;
	}
	return 0;
}
node_t *find_list_end(uint16_t chid)
{
	int32_t i;
	node_t *n = NULL;
	lds_t *s = NULL;
	/** 到表里查找通道*/
	for(i = 0; i < MAX_CH; i++) {
		s = get_local_data_src_handle(i);
		if(s->chid == chid) {
			n = s->head;
			//printf("[find_list_end] head : [%p]\n", n);
			while(n->next){
				n = n ->next;
				//printf("[find_list_end] n : [%p]\n", n);
			}
		}
	}
	return n;
}

int32_t find_chid_index(int32_t chid)
{
	int32_t i = 0;
	lds_t *s = NULL;
	/** 到表里查找通道*/
	for(i = 0; i < MAX_CH; i++) {
		s = get_local_data_src_handle(i);
		if(s->chid == chid) {
			return i;
		}
	}
	return -1;
}

uint16_t is_valid_channel_id(uint16_t chid)
{
	int i;
	lds_t *s = NULL;
	/** 到表里查找通道*/
	for(i = 0; i < MAX_CH; i++) {
		s = get_local_data_src_handle(i);
		if(s->chid == chid) {
			return 1;
		}
	}
	return 0;
}

int32_t get_valid_channel_id(void)
{
	int i;
	lds_t *s = NULL;
	/** 到表里查找通道*/
	for(i = 0; i < MAX_CH; i++) {
		s = get_local_data_src_handle(i);
		if(s->chid != -1) {
			return s->chid;
		}
	}
	return -1;
}




