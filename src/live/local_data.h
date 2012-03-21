#ifndef	__LOCAL_DATA_H__
#define	__LOCAL_DATA_H__
#include "stdint.h"
#include "reach_list.h"
#include "media_msg.h"
typedef struct local_data_srcs {
	int32_t chid;
	node_t *head;
	int32_t list_len;
} lds_t;

lds_t	 lds[MAX_CH];

int32_t get_list_len(int32_t index);
int32_t set_list_len(int32_t index, int32_t list_len);
int32_t get_channel_id(int32_t index);
int32_t set_channel_id(int32_t index, int32_t id);

node_t *get_list_head(int32_t index);
node_t *set_list_head(int32_t index, node_t *head);
node_t *find_list_end(uint16_t chid);
node_t *find_list_head(uint16_t chid);

void init_local_data_src(void);
int32_t get_local_data_index(int32_t chid);
int32_t find_chid_index(int32_t chid);
uint16_t is_valid_channel_id(uint16_t chid);
int32_t get_valid_channel_id(void);
int32_t find_list_node(node_t *n, uint16_t chid);
#endif	//__LOCAL_DATA_H__

