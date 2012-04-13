#ifndef __FILE_MESSAGE_H
#define __FILE_MESSAGE_H


#define IP_INFO_LEN						(16)	//���IP��ַ�ĳ���
#define CHANNEL_CONNECT_MAX_NUM		(100)		//���ͨ��������


#define OPERATION_CONTINUE				(1) 	// ��������
#define OPERATION_SUCC					(0)  	//�����ɹ�����
#define OPERATION_ERR					(-1)	//����ʧ������
#define PASSWD_VERIFICATION_ERR			(-2)	//������֤����
#define CREATE_THREAD_ERR				(-3)	//�����߳�ʧ��
#define TRANSMISSION_DATA_ERR			(-4)	//��������ʧ��
#define OPEN_DEV_ERR						(-5)	//���豸ʧ��
#define MESSAGE_TYPE_ERR					(-6)	//��Ϣ���ʹ���
#define SERVER_IS_CLOLSED				(-8) // ¼�������ر�
#define SERVER_IS_CLOLSED_UNUSUAL 		(-9) //¼���쳣�ر� 

/*************************����״̬g_connect_info ********************/
#define BC_CONNECT_OK					2
#define BC_CONNECTING   					1
#define BC_CONNECT_CLOSE   				0

#define HTTP_CON_STATUS_CHANGED  		1
#define HTTP_CON_STATUS_SLEEP        		0

#define TCP_BT_RECONNECT  				 1          // broadcast reconnet ...
#define  UDP_NO_DATA_MAX    				50       // UDP����50��δ���ܵ�����

/**************************����Э���й�����********************/

#define TCP_START_PORT    					(3000)	//���ӷ�����tcp��ʼ�˿�
#define AVIIF_KEYFRAME 					(0x000010L)
#define MAX_ROOM_BOX					6		//ͬһ��ͨ����������������.
/*�ȴ�socket���ճ�ʱʱ��*/
#define WAIT_RECV_TIMEOUT  				3

#define UDP_CHANNLE_NUM 					1
#define MSG_VER		       					1
#define MSG_TYPE_INFO	   				1
#define MSG_TYPE_HEART	   				2
#define MSG_TYPE_PASSWORD  				3
#define MSG_OPEN_FILE      					4      //VOD�ļ���
#define MSG_VOD_START      					5      //��ʼVOD
#define MSG_VOD_STOP       					6      //ֹͣVOD
#define MSG_VOD_PAUSE      					7      //��ͣ
#define MSG_VOD_TIME       					8      //��ʱ�䲥��
#define MSG_TIMER          					9      //��ʱ��
#define MSG_READ_VIDEO     				10     //��ͼ������
#define MSG_READ_AUDIO     				11     //����������
#define MSG_READ_DATA      					12     //����������
#define MSG_HEART_BEAT     				13     //������Ϣ
#define MSG_FILE_LIST      					14     //�ļ��б�
#define MSG_PC_DATA        					15     //pc����

#define MSG_TYPE_DATAREQ        			14		//�Ƿ���Ҫ����������ý������
#define MSG_TYPE_FIREWALL				7
#define MSG_VOD_PWD						16		//�㲥����  ��ӦsData=0 OK sData=1 ���� sData=2 û�е㲥����
#define MSG_VOD_PLAY_FILE				18		//�㲥�ļ�	��ӦsData=0 OK(���FileInfo) sData=1����
#define MSG_VOD_STOP_FILE				19		//ֹͣ����
#define MSG_VOD_PAUSE_FILE				20		//��ͣ����
#define MSG_VOD_GOTO_FILE				21		//����ָ����ʱ�䲥��
#define MSG_VOD_RESTART_FILE				23		//�ָ�����
#define MSG_VOD_PLAY_END				24		//���Ž���
#define MSG_VOD_MARK_INFO				25	  	//����Mark��Ϣ
#define MSG_FIRE_WALL   					35
#define MSG_TYPE_DATA					36		//VGA����
#define MSG_TYPE_USEINFO					40		//RecPlayer�û����� �û����� 1 RecServer, 0 Player
#define MSG_TYPE_USERLIST				41		//�û��б�0 �����б� 1�¼��û���2�û��˳�
#define MSG_TYPE_BDPROXY				42		//�鲥���� 0 �ر� 1��
#define MSG_TYPE_BDSTATUS				43		//��ѯ�鲥����״̬ + Int �����ѯ��UserID 0 Ϊ���С�
#define MSG_TYPE_CHAT					44		//��������
#define MSG_TYPE_TRANSFILE				45		//�����ļ���
#define MSG_TYPE_CALLNAME				46		//����
#define MSG_TYPE_CLOSEUSER				47		//�ر��û� �� UserID
#define MSG_TYPE_CHATMNG				48		//������رջػ�
#define MSG_TYPE_CNNTINFO  				50
#define MSG_NEW_HEART       				51
#define MSG_TYPE_GETDATA       				52 //���󱾵ػ�Զ������
#define MSG_TYPE_AUDIODATA      			53 //��Ƶ����

#define MAX_USER_NAME					64
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                      \
		((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |   \
		((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))

#define USER_RECPLY						0
#define USER_RECSEV						1

#define	MAX_LEN_PASSWD					10			//���볤��
#define	UDP_START_PORT					(7000)


typedef struct msg_user_infos
{
	int32_t 	m_type;
	uint32_t 	m_local_ip;
	int8_t 		m_user_name[MAX_USER_NAME];
} msg_user_info_t;

//UDP_socket �������ID��Ӧ��
typedef struct udp_socket_channelID_infos
{

	int32_t    		udp_socket[MAX_ROOM_BOX];
	int32_t              m_channelID;
	int32_t              m_num_udp_socket;
	
}udp_socket_channelID_info_t;
//Ҫ���ӵķ�������Ϣ�����ݽṹ
typedef struct connect_server_infos
{
	int8_t 		m_ip_addr[IP_INFO_LEN];				//Ҫ���ӵķ�����IP
	int32_t	 	m_channel;							//ͨ����.
	int32_t		m_udp_start_port;						//UDP��ʼ�˿ں�.
	int32_t		m_tcp_start_port;						//TCP��ʼ�˿ں�.
	int32_t		m_num_encode;						//��ͨ�����ӵı���������.
	int32_t		m_connect_status;						//����״̬,0��ʾ�Ͽ�,1��ʾ����.
	int32_t		m_reconnect_flag;						//������־,0��ʾ������,������ʾһֱ����.
	int8_t		m_passwd_info[MAX_LEN_PASSWD];		//���ӷ���������
	int32_t		m_tcp_socket;							//����tcp���ӵ�socket.
	int32_t		m_udp_socket[MAX_ROOM_BOX];			//����udp���ӵ�socket.
	int32_t		m_id;								//�����ұ��
	pthread_t   m_connect_thread;						//�������������̵߳ľ��.
	pthread_t   m_heart_thread;							//���������߳̾��.
	int32_t  		m_state_change;    						// ��Ǵ�λ��Ԫ���Ƿ�ı�
	int32_t  		m_bt_connect;                                 		 	// braodcast reconnect
	struct  timeval  time_udp_no_data;                        	 		// ���UDP���ٴ�û��UDP���ݽ���

}connect_server_info_t;

typedef struct web_cmd_parse_infos
{
	int32_t	channel_id;					//���ӽڵ�(������)�ı��
	int32_t	rec_channel;				//¼����������Ӧ��ͨ����
	int8_t	rec_ip_addr[IP_INFO_LEN];	//Ҫ���ӵķ�������IP��ַ
}web_cmd_parse_info_t;


int32_t	init_recv_data_thread(void);
int32_t 	live_rec_server_init(connect_server_info_t *connect_info, web_cmd_parse_info_t *cmd_info);
int32_t 	live_rec_server_connect(connect_server_info_t *connect_info);
int32_t	 tim_subtract(struct timeval *result, struct timeval *x, struct timeval *y);
void 		init_select_udp_array(udp_socket_channelID_info_t socket_channelID[]);
int32_t 	set_select_udp_array(udp_socket_channelID_info_t socket_channelID[],connect_server_info_t connect_info,fd_set *fdsr);
int32_t 	deal_select_udp_array(udp_socket_channelID_info_t *socket_channelID,connect_server_info_t * connect_info_p, fd_set *fdsr, int32_t msg_data_socket);
void 		g_connect_info_init();
int32_t 	broadcast_udp_check_online(connect_server_info_t *connect_info,int32_t id );
int32_t 	live_rec_server_exit(connect_server_info_t *connect_info);
void 		g_connect_info_init();
#endif

