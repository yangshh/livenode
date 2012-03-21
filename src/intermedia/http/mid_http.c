#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdint.h"
#include "curl.h"
#include "mid_http.h"
//#include "./liblog/include/log_common.h"


int32_t recv_long_tcp_data_extend(int32_t socket,void* buffer,const int32_t len)
{
	int32_t total_recv = 0;
	int32_t recv_len = 0;
	while(total_recv < len)
	{
		recv_len = recv(socket, buffer + total_recv, len - total_recv, 0);
		if(recv_len < 1)
		{
			if(recv_len != 0)
			{
				printf("recv tcp data failed    --- [%d]\n",recv_len);
			}
			return -1;
		}		
		total_recv += recv_len;
		if(total_recv == len)
		{
			break;
		}
	}
	return total_recv;
}


static size_t write_callback_header(char *buffer, size_t size,size_t nitems,void *outstream)
{
	struct HTTP_mid *temp_outstream = (struct HTTP_mid*)outstream;
	int outstream_need_len = strlen((char *)temp_outstream->out_buf) + strlen((char *)buffer);
//	printf("outstream_need_len is --- [%d]\n",outstream_need_len);
	if(temp_outstream->buf_max_len < outstream_need_len -1 )
	{
		printf("[write_callback_header]outstream_len is samll!\n");
		*(temp_outstream->out_buf_len) = -1;
		return 0;
	}	
	else
	{
		strcat(temp_outstream->out_buf,buffer);
	}			
	*(temp_outstream->out_buf_len) = outstream_need_len ;
  return nitems;
}

static size_t write_callback_other(char *buffer,size_t size,size_t nitems,void *outstream)
{
  struct HTTP_mid *temp_outstream = (struct HTTP_mid*)outstream;
	int outstream_need_len = strlen((char *)temp_outstream->out_buf) + strlen((char *)buffer);
//	printf("outstream_need_len is --- [%d]\n",outstream_need_len);
	if(temp_outstream->buf_max_len < outstream_need_len -1 )
	{
		printf("[write_callback_other]outstream_len is samll!\n");
		*(temp_outstream->out_buf_len) = -1;
		return 0;
	}	
	else
	{
		strcat(temp_outstream->out_buf,buffer);
	}			
	*(temp_outstream->out_buf_len) = outstream_need_len ;
  return nitems;
}


int mid_http_get(char *url, char *info, int inlen, char *output, int *outlen)
{
	int http_body_len = 0;
  	char http_head_buf[HTTP_HEAD_LEN_MAX] = {0};
	int http_head_len = 0;
  	CURL *curl;
  	CURLcode res;
  	int32_t i_loop = 0 ;
  	int32_t recode = 0;
	int32_t return_code = -1;
  
  if(output == NULL)
  {
  	return -1;     // 传入output指针有问题 
  }
  
  struct HTTP_mid http_body;
  http_body.out_buf_len= &http_body_len;
  http_body.buf_max_len = *outlen;
  http_body.out_buf = output;
  
  struct HTTP_mid http_head;
  http_head.out_buf_len = &http_head_len;
  http_head.buf_max_len = HTTP_HEAD_LEN_MAX;
  http_head.out_buf = http_head_buf;
  
  curl = curl_easy_init();
  if(curl)
   {
  	 curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_POST_TIME);
  	 curl_easy_setopt(curl, CURLOPT_WRITEHEADER,&http_head);
  	 curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_callback_header);
  	 curl_easy_setopt(curl, CURLOPT_FILE,(FILE*)&http_body);
  	 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_other);
  	 curl_easy_setopt(curl, CURLOPT_URL, url);
 		 res = curl_easy_perform(curl);
 		 
	    if(CURLE_OK != res)
	   {
	   		printf("CURLE_OK != res ---> ");
	   		printf("http_get_curl_recode is --- [%d]\n",res);
	    	goto EXIT;
	   }
			if(*(http_head.out_buf_len) != -1)
			{
				if(strncmp(http_head.out_buf,HTTP_VERSION_1,HTTP_VERSION_LEN) == 0 || strncmp(http_head.out_buf,HTTP_VERSION_2,HTTP_VERSION_LEN) == 0)
				{
					i_loop = HTTP_VERSION_LEN;
					while(http_head.out_buf[i_loop] == ' ')
					{
						i_loop ++ ;
					}
					recode = atoi(&http_head.out_buf[i_loop]);
					if(recode == 0)
					{
						printf("ATOI is error!\n");
						goto EXIT;
					}
					if(recode != HTTP_OK)
					{
						printf("HTTP_RECODE_NUM_get IS ---- [%d]\n",recode);
						goto EXIT;
					}				
				}
				else
				{
					printf("GET HTTP_HEAD_VERSION IS error!\n");
					goto EXIT;
				}				
			}
			else
			{
				printf("GET HTTP_HEAD LEN IS error!\n");
				goto EXIT;
			}
		 if(*(http_body.out_buf_len) != -1)
	   {
	   			*outlen = *(http_body.out_buf_len);
		}
		else
		{
			printf("GET HTTP_BODY LEN IS error!\n");
			goto EXIT;
		}
	}		
  return_code =  0;
EXIT:
	curl_easy_cleanup(curl);
  	curl=NULL;
	return return_code; 
  /*info  inlen 暂时空置*/
}

int mid_http_post(char *url, char *info, int inlen, char *output, int *outlen)
{
	int http_body_len = 0;
  	char http_head_buf[HTTP_HEAD_LEN_MAX] = {0};
	int http_head_len = 0;
  	CURL *curl = NULL;
  	CURLcode res;
  	int i_loop = 0 ;
  	int32_t recode = 0;
	int32_t return_code = -1;
	
if(output == NULL)
  {
  	printf("output == NULL");
  	return -1;     // 传入output指针有问题 
  }
  
 struct HTTP_mid http_body;
  http_body.out_buf_len= &http_body_len;
  http_body.buf_max_len = *outlen;
  http_body.out_buf = output;
  
  struct HTTP_mid http_head;
  http_head.out_buf_len = &http_head_len;
  http_head.buf_max_len = HTTP_HEAD_LEN_MAX;
  http_head.out_buf = http_head_buf;
  
  curl = curl_easy_init();
 
  if(curl)
   {
  
  	 curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_POST_TIME);
  	 curl_easy_setopt(curl, CURLOPT_WRITEHEADER,&http_head);
  	 curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_callback_header);
  	 curl_easy_setopt(curl, CURLOPT_FILE,(FILE*)&http_body);
  	 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_other);
  	 curl_easy_setopt(curl, CURLOPT_URL,url);
  	 curl_easy_setopt(curl, CURLOPT_POSTFIELDS, info);
 	 res = curl_easy_perform(curl);
 		 
	   if(CURLE_OK != res)
	   {
	   		printf("CURLE_OK != res --->");
	   		printf("http_post_curl_recode is --- [%d]\n",res);
	    	goto EXIT;
	   }
	if(*(http_head.out_buf_len) != -1)
	{
		if(strncmp(http_head.out_buf,HTTP_VERSION_1,HTTP_VERSION_LEN) == 0 || strncmp(http_head.out_buf,HTTP_VERSION_2,HTTP_VERSION_LEN) == 0)
		{
			i_loop = HTTP_VERSION_LEN;
			while(http_head.out_buf[i_loop] == ' ')
			{
				i_loop ++ ;
			}
			recode = atoi(&http_head.out_buf[i_loop]);
			if(recode == 0)
			{
				printf("ATOI is error!\n");
				goto EXIT;
			}

			if(recode != HTTP_OK )
			{
				printf("HTTP_RECODE_NUM IS_post ---- [%d]\n",recode);
				goto EXIT;
			}				
		}
		else
		{
					printf("GET HTTP_HEAD_VERSION IS error!\n");
					goto EXIT;
		
		}				
			}
	else
	{
		printf("GET HTTP_HEAD LEN IS error!\n");
		goto EXIT;
	}
	 if(*(http_body.out_buf_len) != -1)
	  {
	    	*outlen = *(http_body.out_buf_len);
	}
	else
	{
		printf("GET HTTP_BODY LEN IS error!\n");
		goto EXIT;
	}
}		
  return_code =  0;
EXIT:
	curl_easy_cleanup(curl);
  	curl=NULL;
	return return_code; 
}

#if 0
int main()
{
	char buf[10000] = {0};
	int buf_len = 10000;
//	char buf_post[100] = "god is a girl!";
//	int buf_post_len = strlen(buf_post);
	int return_num = mid_http_get(HTTP_TEST_WEB_URL,NULL,0,&buf,&buf_len);
//	int return_num_poost = mid_http_post("http://192.168.6.15:8080/mytest2/HttpXmlMsgAction",&buf_post,buf_post_len,&buf,&buf_len);
	printf("**************************\n");
	if(return_num == 0)
	{
    	printf("%s ---- %d  \n",buf,buf_len);
	}
	return 0;
}
#endif


