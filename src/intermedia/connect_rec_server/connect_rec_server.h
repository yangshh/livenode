#ifndef __FILE_MESSAGE_H
#define __FILE_MESSAGE_H


#define IP_INFO_LEN						(16)	//存放IP地址的长度
#define CHANNEL_CONNECT_MAX_NUM		(100)		//最大通道连接数


#define OPERATION_CONTINUE				(1) 	// 操作继续
#define OPERATION_SUCC					(0)  	//操作成功跳出
#define OPERATION_ERR					(-1)	//操作失败跳出
#define PASSWD_VERIFICATION_ERR			(-2)	//密码验证错误
#define CREATE_THREAD_ERR				(-3)	//创建线程失败
#define TRANSMISSION_DATA_ERR			(-4)	//传输数据失败
#define OPEN_DEV_ERR						(-5)	//打开设备失败
#define MESSAGE_TYPE_ERR					(-6)	//消息类型错误
#define SERVER_IS_CLOLSED				(-8) // 录播正常关闭
#define SERVER_IS_CLOLSED_UNUSUAL 		(-9) //录播异常关闭 

/*************************连接状态g_connect_info ********************/
#define BC_CONNECT_OK					2
#define BC_CONNECTING   					1
#define BC_CONNECT_CLOSE   				0

#define HTTP_CON_STATUS_CHANGED  		1
#define HTTP_CON_STATUS_SLEEP        		0

#define TCP_BT_RECONNECT  				 1          // broadcast reconnet ...
#define  UDP_NO_DATA_MAX    				50       // UDP数据50次未接受到数据

/**************************接收协议有关内容********************/

#define TCP_START_PORT    					(3000)	//连接服务器tcp起始端口
#define AVIIF_KEYFRAME 					(0x000010L)
#define MAX_ROOM_BOX					6		//同一个通道中最多编码器个数.
/*等待socket接收超时时间*/
#define WAIT_RECV_TIMEOUT  				3

#define UDP_CHANNLE_NUM 					1
#define MSG_VER		       					1
#define MSG_TYPE_INFO	   				1
#define MSG_TYPE_HEART	   				2
#define MSG_TYPE_PASSWORD  				3
#define MSG_OPEN_FILE      					4      //VOD文件名
#define MSG_VOD_START      					5      //开始VOD
#define MSG_VOD_STOP       					6      //停止VOD
#define MSG_VOD_PAUSE      					7      //暂停
#define MSG_VOD_TIME       					8      //按时间播放
#define MSG_TIMER          					9      //定时器
#define MSG_READ_VIDEO     				10     //读图象数据
#define MSG_READ_AUDIO     				11     //读语音数据
#define MSG_READ_DATA      					12     //读桌面数据
#define MSG_HEART_BEAT     				13     //心跳消息
#define MSG_FILE_LIST      					14     //文件列表
#define MSG_PC_DATA        					15     //pc数据

#define MSG_TYPE_DATAREQ        			14		//是否需要服务器发送媒体数据
#define MSG_TYPE_FIREWALL				7
#define MSG_VOD_PWD						16		//点播密码  回应sData=0 OK sData=1 错误 sData=2 没有点播功能
#define MSG_VOD_PLAY_FILE				18		//点播文件	回应sData=0 OK(后跟FileInfo) sData=1错误
#define MSG_VOD_STOP_FILE				19		//停止播放
#define MSG_VOD_PAUSE_FILE				20		//暂停播放
#define MSG_VOD_GOTO_FILE				21		//跳到指定的时间播放
#define MSG_VOD_RESTART_FILE				23		//恢复播放
#define MSG_VOD_PLAY_END				24		//播放结束
#define MSG_VOD_MARK_INFO				25	  	//发送Mark信息
#define MSG_FIRE_WALL   					35
#define MSG_TYPE_DATA					36		//VGA数据
#define MSG_TYPE_USEINFO					40		//RecPlayer用户名， 用户种类 1 RecServer, 0 Player
#define MSG_TYPE_USERLIST				41		//用户列表。0 完整列表， 1新加用户、2用户退出
#define MSG_TYPE_BDPROXY				42		//组播代理 0 关闭 1打开
#define MSG_TYPE_BDSTATUS				43		//查询组播代理状态 + Int 代表查询的UserID 0 为所有。
#define MSG_TYPE_CHAT					44		//聊天数据
#define MSG_TYPE_TRANSFILE				45		//传输文件。
#define MSG_TYPE_CALLNAME				46		//点名
#define MSG_TYPE_CLOSEUSER				47		//关闭用户 ＋ UserID
#define MSG_TYPE_CHATMNG				48		//开启或关闭回话
#define MSG_TYPE_CNNTINFO  				50
#define MSG_NEW_HEART       				51
#define MSG_TYPE_GETDATA       				52 //请求本地或远端数据
#define MSG_TYPE_AUDIODATA      			53 //音频数据

#define MAX_USER_NAME					64
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                      \
		((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |   \
		((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))

#define USER_RECPLY						0
#define USER_RECSEV						1

#define	MAX_LEN_PASSWD					10			//密码长度
#define	UDP_START_PORT					(7000)


typedef struct msg_user_infos
{
	int32_t 	m_type;
	uint32_t 	m_local_ip;
	int8_t 		m_user_name[MAX_USER_NAME];
} msg_user_info_t;

//UDP_socket 与会议室ID对应表
typedef struct udp_socket_channelID_infos
{

	int32_t    		udp_socket[MAX_ROOM_BOX];
	int32_t              m_channelID;
	int32_t              m_num_udp_socket;
	
}udp_socket_channelID_info_t;
//要连接的服务器信息的数据结构
typedef struct connect_server_infos
{
	int8_t 		m_ip_addr[IP_INFO_LEN];				//要连接的服务器IP
	int32_t	 	m_channel;							//通道号.
	int32_t		m_udp_start_port;						//UDP起始端口号.
	int32_t		m_tcp_start_port;						//TCP起始端口号.
	int32_t		m_num_encode;						//本通道连接的编码器个数.
	int32_t		m_connect_status;						//连接状态,0表示断开,1表示连接.
	int32_t		m_reconnect_flag;						//重连标志,0标示不重连,其它表示一直重连.
	int8_t		m_passwd_info[MAX_LEN_PASSWD];		//连接服务器密码
	int32_t		m_tcp_socket;							//创建tcp连接的socket.
	int32_t		m_udp_socket[MAX_ROOM_BOX];			//创建udp连接的socket.
	int32_t		m_id;								//会议室编号
	pthread_t   m_connect_thread;						//创建网络连接线程的句柄.
	pthread_t   m_heart_thread;							//创建心跳线程句柄.
	int32_t  		m_state_change;    						// 标记此位置元素是否改变
	int32_t  		m_bt_connect;                                 		 	// braodcast reconnect
	struct  timeval  time_udp_no_data;                        	 		// 标记UDP多少次没有UDP数据交互

}connect_server_info_t;

typedef struct web_cmd_parse_infos
{
	int32_t	channel_id;					//连接节点(会议室)的编号
	int32_t	rec_channel;				//录播服务器对应的通道号
	int8_t	rec_ip_addr[IP_INFO_LEN];	//要连接的服务器的IP地址
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

