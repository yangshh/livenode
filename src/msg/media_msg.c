
#include "media_msg.h"
#include "reach_os.h"
/*消息头打包*/
void pack_header_msg(void *data, uint8_t type, uint16_t len)
{
	msg_header_t  *p;
	p=(msg_header_t *)data;
	r_memset(p, 0x00, sizeof(msg_header_t));
	p->m_len=htons(len);
	p->m_msg_type = type;
}

void pack_unite_user_info(net_user_info_t *nu)
{
        nu->uflag =  USER_FLAG_UNITE;
        r_strcpy(nu->uname, USER_NAME);
        r_strcpy(nu->upasswd, UNITE_PASSWD);
}

int32_t check_net_user(net_user_info_t *nu)
{
	printf("[check_net_user]flag : [%d], name : [%s] passwd : [%s]\n", nu->uflag, nu->uname, nu->upasswd);
	if(USER_FLAG_PLAYER == nu->uflag) {
		if (r_strcmp((const int8_t *)nu->uname, (const int8_t *)USER_NAME) == 0 &&
			r_strcmp((const int8_t *)nu->upasswd, (const int8_t *)USER_PASSWD) == 0)
			return USER_LOGIN_VALID;
		else
			return USER_LOGIN_INVALID;
	}
	return USER_LOGIN_INVALID;
}


int32_t write_file(char *file, void *buf, int buflen)
{
	static FILE *fp = NULL;
	if(NULL == fp) {
		fp =	fopen(file, "w");
		if(NULL == fp) {
			perror("fopen ");
			return -1;
		}
	}
	printf("[write_file] buf : [%p], buflen : [%d] , fp : [%p]\n", buf, buflen, fp);
	fwrite(buf, buflen, 1, fp);
	return 0;
}

void init_channel_infos(channel_info_t *cit)
{
	cit->m_channel = -1;
	cit->m_data = NULL;
	cit->m_data_len = 0;
	cit->m_msg_flag = 0;
	cit->m_msg_type = -1;
}


