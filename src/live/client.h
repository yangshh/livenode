#ifndef	__LOCAL_CLIENT_H__
#define	__LOCAL_CLIENT_H__

#include "stdint.h"
#include "user.h"

enum send_status {
	NOT_SENT = -1,
	NOT_AFTER_SENDING = 0,
	IS_SENDING = 1,
	NOT_CAN_SEND = 2
};
int32_t get_node_data_len(void *data);
int32_t local_send_process(user_t *u);
int32_t local_recv_process(user_t *u);

int32_t net_send_process(user_t *u);
int32_t net_recv_process(user_t *u);
int32_t send_user_process(void);

#endif	//__LOCAL_CLIENT_H__


