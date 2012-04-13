#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "reach_os.h"
#include "stdint.h"
#include "reach_list.h"
#include "local_data.h"
#include "user.h"
#include "connect_rec_server.h"
#include "web_protocol_resolve.h"

#include "events_process.h"
#include "silverlight_policy.h"
#include "version.h"

#if __WORDSIZE == 64
#define	ARCH	"x86_64"
#else
#define	ARCH	"i686"
#endif


int32_t run_live_data_send(void)
{
	pthread_t   ldsp;
	//创建和发送交换的线程
	if (r_pthread_create(&ldsp, NULL, 
		events_process, NULL)) 
	{
		printf("Failed to create live_data_send thread thread = %s\n",strerror(errno));
		return -1;	
	}
	return 0;
}

int32_t run_silverlight_policy(void)
{
	pthread_t sp;
	
	//创建和发送交换的线程
	if (r_pthread_create(&sp, NULL, 
						silverlight_policy_process, NULL)) 
	{
		printf("Failed to create live_data_send thread thread = %s\n",strerror(errno));
		return -1;	
	}
	return 0;
}

void run_live(void)
{
	Signal(SIGPIPE,SIG_IGN);
	init_local_data_src();
	init_client_info_table();
	init_recv_data_thread();
	init_web_protoclo_resolve_func();
	run_silverlight_policy();
	run_live_data_send();
	while(1) {
		sleep(1);
	}
}

/*
##子进程捕捉回调函数
*/
void sig_child(int signo)
{	
	pid_t	pid;
	int 	stat;
	
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) 
	{
		printf("child %d terminated\n", pid);
		pid = fork();
		if(pid < 0) 
		{
			printf("fork error\n");
			exit(1);
		}
		usleep(1000000);
		if(pid == 0)  
		{  //child
			usleep(1000000);
			run_live();
		}		
	}
	return;
}

void print_live_version_info(void)
{
	printf("/*****************************************/\n\n");
	printf("/* <SZ Reach Tech Co.,Ltd>\n\n");
	printf("/* [lives.v%s.%s.build%d]\n\n", _VERSION, ARCH, BDATE);
	printf("/*****************************************/\n\n");
}

int  main(int argc, char **argv)
{
	print_live_version_info();
	pid_t 			pid 			= 0;
	pid = fork();
	if(pid < 0) {
		printf("fork error\n");
		exit(1);
	}
	if(pid == 0)  {  //child
		run_live();
	}  else {	
		Signal(SIGCHLD, sig_child);	
		while(1)
 		{
			sleep(1);
		}
	}
	return 0;
}

