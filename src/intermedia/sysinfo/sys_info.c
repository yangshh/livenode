#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_info.h"
int run_cpu()
{
	FILE *fp = NULL;
	char *szLine = NULL;
	size_t len = 0;
	int cpu_percent_used = 0;
	
	unsigned long long cpu_total = 0;
	unsigned long long cpu_total_used = 0;
	unsigned long long pre_cpu_total = 0;
	unsigned long long pre_cpu_total_used = 0;
	
	unsigned long long normal_user = 0;
	unsigned long long nice_user = 0;
	unsigned long long system = 0;
	unsigned long long idle = 0;
	unsigned long long iowait = 0;
	unsigned long long irq = 0;
	unsigned long long softirq = 0;

	fp = fopen("/proc/stat", "r");

	if( fp == NULL ) {
		printf("[%s]---run_cpu: open /proc/stat error",__func__);
		return -1;
	}
	//fgets(char *buf，int len，stdin);
	if(getline(&szLine, &len, fp) < 0 )
	{
		printf("[%s]---run_cpu: read /proc/stat error",__func__);
		fclose(fp);
		return -1;
	}
	
	sscanf(szLine, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu", 
                        &normal_user, &nice_user,
                        &system, &idle,
                        &iowait, &irq, &softirq );
	
	pre_cpu_total_used = normal_user + nice_user + system + iowait + irq + softirq;
	pre_cpu_total = idle + pre_cpu_total_used;
	printf("[%s]---idle is  -- [%Lu]",__func__,idle);
	printf("pre_cpu_total_used is --[%Lu]  pre_cpu_total is --[%Lu]\n",pre_cpu_total_used,pre_cpu_total);
	
	if(szLine)
		free(szLine);
	if(fp)
		fclose(fp);
	fp = NULL;
	szLine = NULL;
	len = 0;
	
	usleep(100000);
	normal_user = 0;
	nice_user = 0;
	system = 0;
	idle = 0;
	iowait = 0;
	irq = 0;
	softirq = 0;
	
	fp = fopen("/proc/stat", "r");

	if( fp == NULL ) {
		printf("[%s]---run_cpu: open /proc/stat error",__func__);
		return -1;
	}
	
	if( getline(&szLine, &len, fp) < 0 ) {
		printf("[%s]---run_cpu: read /proc/stat error",__func__);
		fclose(fp);
		return -1;
	}
	sscanf(szLine, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu", 
                        &normal_user, &nice_user,
                        &system, &idle,
                        &iowait, &irq, &softirq );
	
	cpu_total_used = normal_user + nice_user + system + iowait + irq + softirq;
	cpu_total = idle + cpu_total_used;
	printf("[%s]---idle is  -- [%Lu]",__func__,idle);
	printf("[%s]---cpu_total_used is --[%Lu]  cpu_total is --[%Lu]\n",__func__,cpu_total_used,cpu_total);
	//计算CPU使用率，这是前一个时间间隔内的平均使用率
	cpu_percent_used = (int)ceil( 100 * ((float)(cpu_total_used-pre_cpu_total_used)) / (cpu_total-pre_cpu_total) );
	if(szLine)
		free(szLine);
	if(fp)
		fclose(fp);
	
	return cpu_percent_used;
	
}
//获取内存使用信息
int run_memory()
{
	int mem_percent_used = 0;
	int mem_total = 0;
	int mem_free = 0;
	FILE *fp = NULL;
	char *szLine = NULL;
	size_t len = 0;
	int nTotal = 0;
	int nFree = 0;
	int nBuffers = 0;
	int nCached = 0;
	int nSwapCached = 0;
	fp = fopen("/proc/meminfo", "r");
	if( fp == NULL ) {
		printf("[%s]---run_memory: open /proc/meminfo error",__func__);
		return -1;
	}
	
	while(!feof(fp)) {
		if(szLine) {
			free(szLine);
			szLine = NULL;
		}
		if( getline(&szLine, &len, fp) < 0) {
			if(feof(fp))
				continue;
			printf("[%s]---run_memory: read /proc/meminfo error",__func__);
			fclose(fp);
			return -1;
		}
		if( strncmp(szLine, "MemTotal", strlen("MemTotal")) == 0 ) {
			sscanf(szLine, "MemTotal: %d", &nTotal);
		}
		else if( strncmp(szLine, "MemFree", strlen("MemFree")) == 0 ) {
			sscanf(szLine, "MemFree: %d", &nFree);
		}
		else if( strncmp(szLine, "Buffers", strlen("Buffers")) == 0 ) {
			sscanf(szLine, "Buffers: %d", &nBuffers);
		}
		else if( strncmp(szLine, "Cached", strlen("Cached")) == 0 ) {
			sscanf(szLine, "Cached: %d", &nCached);
		}
		else if( strncmp(szLine, "SwapCached", strlen("SwapCached")) == 0 ) {
			sscanf(szLine, "SwapCached: %d", &nSwapCached);
			break;
		}
		else
			continue;
	}
	//更新
	mem_total = nTotal;
	mem_free = nFree + nBuffers + nCached + nSwapCached;
  mem_percent_used = (int)ceil(100*((float)(mem_total-mem_free)/mem_total));
	printf("[%s]---mem_free is -- [%d]\n",__func__,mem_free);
	if(szLine)
		free(szLine);
	if(fp)
		fclose(fp);
		
	return mem_percent_used;
}

unsigned long long run_throughput()
{
	FILE *fp = NULL;
	char *szLine = NULL;
	size_t len = 0;

	unsigned long long cpu_net_num =0;
	
	unsigned long long nEth0In = 0;
	unsigned long long nEth0Out = 0;
	unsigned long long nEth1In = 0;
	unsigned long long nEth1Out = 0;
	unsigned long long nouse = 0;

	fp = fopen("/proc/net/dev", "r");
	if( fp == NULL ) {
		printf("[%s]---run_throughput: open /proc/net/dev error",__func__);
		return -1;
	}
	while(!feof(fp)) {
		char *p = NULL;

		if(szLine) {
			free(szLine);
			szLine = NULL;
		}
		if( getline(&szLine, &len, fp) < 0 ) {
			if(feof(fp))
				continue;
			printf("[%s]---run_throughput: read /proc/net/dev error",__func__);
			fclose(fp);
			return -1 ;
		}
		//忽略每行开头的空格
		p = szLine;
		while( *p && (*p==' ') )
			p++;
		if( strncmp(p, "eth0", strlen("eth0")) == 0 ) {
			sscanf(p, "eth0:%Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu", 
				&nEth0In,
				&nouse, &nouse, &nouse, &nouse, &nouse, &nouse, &nouse, 
				&nEth0Out );
		}
		else if( strncmp(p, "eth1", strlen("eth1")) == 0 ) {
			sscanf(p, "eth1:%Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu", 
				&nEth1In,
				&nouse, &nouse, &nouse, &nouse, &nouse, &nouse, &nouse, 
				&nEth1Out );
			break;
		}
		else
			continue;
	}
	cpu_net_num = nEth0In+nEth0Out+nEth1In+nEth1Out;
	printf("nEth0In is --[%Lu]  nEth0Out is --[%Lu]\n",nEth0In,nEth0Out);
	if(szLine)
		free(szLine);
	if(fp)
		fclose(fp);
	return cpu_net_num;
}
int run_throughput_boost()
{
	int cpu_net_used = 0;
	unsigned long long cpu_net_1 =0;
	unsigned long long cpu_net_2 =0;
	cpu_net_1 = run_throughput();
	
	usleep(1000000);
	
	cpu_net_2 = run_throughput();
	
	cpu_net_used = (int)ceil(100*(float)(cpu_net_2-cpu_net_1)/125/1024);	

	printf("[%s]---cpu_net_used is  -- [%d]\n",__func__,cpu_net_used);
	return cpu_net_used;
}


#if 0
int main()
{
	int cpu_used = 0;
	int cpu_net = 0 ;
	int cpu_mem = 0;
	cpu_used = run_cpu();
	cpu_net = run_throughput_boost();
	cpu_mem = run_memory();
	
	printf("cpu_used is --[%d] cpu_net  is --[%d] cpu_mem  is --[%d]\n",cpu_used,cpu_net,cpu_mem);
					
	return 0;
}

#endif

