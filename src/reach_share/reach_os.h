
#ifndef __REACH_OS__
#define __REACH_OS__
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <malloc.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#define _XOPEN_SOURCE 600
typedef	void Sigfunc(int);
#define r_snprintf	snprintf
void *r_malloc(size_t size);
void r_free(void *ptr);
void *r_memset(void *s, int32_t c, size_t n);
int8_t *r_strcat(int8_t *dest, const int8_t *src);
int8_t *r_strncat(int8_t *dest, const int8_t *src, size_t n);
void r_bzero(void *s, size_t n);
void *r_memalign(size_t boundary, size_t size);

int32_t r_pthread_create(pthread_t * thread, pthread_attr_t * attr, 
	void * (*start_routine)(void *), void * arg);
ssize_t r_recv(int32_t socket, void *buffer, size_t len, int32_t flags);
ssize_t r_recvfrom(int32_t socket, void *buffer, size_t len, int32_t flags, struct sockaddr *from, socklen_t *from_len);
ssize_t r_send(int32_t socket, const void *buffer, size_t len, int32_t flags);
void *r_memcpy(void *dest, const void *src, size_t n);
void *r_cmemcpy(int8_t *dest, int8_t *src, size_t n, int32_t (*condition)(void *));
int8_t *r_strcpy(int8_t *dest, const int8_t *src);
int32_t r_getsockname(int32_t socket, struct sockaddr *name, socklen_t *name_len);
uint16_t r_htons(uint16_t host_short);
int8_t r_inet_aton(const int8_t *cp, struct in_addr *inp);
ssize_t r_sendto(int32_t socket, const void *buffer, size_t len, int32_t flags, 
	const struct sockaddr *to, socklen_t tolen);
int32_t r_socket(int32_t domain, int32_t type, int32_t protocol);
int32_t r_setsockopt(int32_t socket, int32_t level, int32_t opt_name, const void *opt_val,
	socklen_t opt_len);
size_t r_strlen(const int8_t *s);
int32_t r_connect(int32_t socket, const struct sockaddr *serv_addr, socklen_t addr_len);
int32_t r_close(int32_t fd);
uint16_t r_ntohs(uint16_t net_short);
int32_t r_pthread_detach(pthread_t th);
pthread_t r_pthread_self(void);
 uint32_t r_sleep(int32_t second);
void r_usleep(int32_t u_second);
int32_t r_strcmp(const int8_t *s1, const int8_t *s2);
int32_t r_strncmp(const int8_t *s1, const int8_t *s2, size_t n);
int32_t r_memcmp(const void *s1, const void *s2, size_t n);
Sigfunc *Signal(int32_t signo, Sigfunc *func);
#endif //__REACH_OS__

