
#include "reach_os.h"
#include "stdint.h"
void *r_malloc(size_t size)
{
	 return (void *)malloc(size);
}

void r_free(void *ptr)
{
	free(ptr);
}

void *r_memset(void *s, int32_t c, size_t n)
{
	return memset(s, c, n);
}

int8_t *r_strcat(int8_t *dest, const int8_t *src)
{
	return strcat(dest, src);
}
int8_t *r_strncat(int8_t *dest, const int8_t *src, size_t n)
{
	return strncat(dest, src, n);
}

void r_bzero(void *s, size_t n)
{
	bzero(s, n);
}

void *r_memalign(size_t boundary, size_t size)
{
	return memalign(boundary, size);
}

int32_t r_pthread_create(pthread_t * thread, pthread_attr_t * attr, 
	void * (*start_routine)(void *), void * arg)
{
	return pthread_create(thread, attr, start_routine, arg);
}

ssize_t r_recv(int32_t socket, void *buffer, size_t len, int32_t iFlags)
{
	return recv(socket, buffer, len, iFlags);
}

ssize_t r_recvfrom(int32_t socket, void *buffer, size_t len, int32_t flags, struct sockaddr *from, socklen_t *from_len)
{
	return recvfrom(socket, buffer, len, flags, from, from_len);
}

ssize_t r_send(int32_t socket, const void *buffer, size_t len, int32_t flags)
{
	return send(socket, buffer, len, flags);
}

void *r_memcpy(void *dest, const void *src, size_t n)
{
	return memcpy(dest, src, n);
}

void *r_cmemcpy(int8_t *dest, int8_t *src, size_t n, int32_t (*condition)(void *))
{
	int32_t i;
        int32_t j = 0;
        for(i = 0; i <= n; i ++) {
                if(condition(src)) {
                        dest[j ++] = src[i];
                }
        }
        dest[j ++] = 0;
	return dest;
}

int8_t *r_strcpy(int8_t *dest, const int8_t *src)
{
	return strcpy(dest, src);
}

int32_t r_getsockname(int32_t socket, struct sockaddr *name, socklen_t *name_len)
{
	return getsockname(socket, name, name_len);
}

uint16_t r_htons(uint16_t hostshort)
{
	return htons(hostshort);
}

int8_t r_inet_aton(const int8_t *cp, struct in_addr *inp)
{
	return inet_aton(cp,inp);
}

ssize_t r_sendto(int32_t socket, const void *buffer, size_t len, int32_t flags, 
	const struct sockaddr *to, socklen_t tolen)
{
	return sendto(socket, buffer, len, flags, to,tolen);
}

int32_t r_socket(int32_t domain, int32_t type, int32_t protocol)
{
	return socket(domain, type, protocol);
}

int32_t r_setsockopt(int32_t socket, int32_t level, int32_t opt_name, const void *opt_val, socklen_t opt_len)
{
	return setsockopt(socket,level,opt_name,opt_val,opt_len);
}

size_t r_strlen(const int8_t *s)
{
	return strlen(s);
}

int32_t r_connect(int32_t socket, const struct sockaddr *serv_addr, socklen_t addr_len)
{
	return connect(socket, serv_addr,addr_len);
}
int32_t r_close(int32_t fd)
{
	return close(fd);
}

uint16_t r_ntohs(uint16_t net_short)
{
	return ntohs(net_short);
}

int32_t r_pthread_detach(pthread_t th)
{
	return pthread_detach(th);
}

pthread_t r_pthread_self(void)
{
	return pthread_self();
}

 uint32_t r_sleep(int32_t second)
{
	return sleep(second);
}

void r_usleep(int32_t u_second)
{
	usleep(u_second);
}

int r_strcmp(const int8_t *s1, const int8_t *s2)
{
	return strcmp(s1, s2);
}

int r_strncmp(const int8_t *s1, const int8_t *s2, size_t n)
{
	return strncmp(s1, s2, n);
}

int32_t r_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n);
}


/*捕捉信号量*/
Sigfunc *signal(int signo, Sigfunc *func)
{
	struct sigaction	act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef	SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}

/*信号包裹函数*/
Sigfunc *Signal(int signo, Sigfunc *func)
{
	Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
	{
		printf("ERROR:  signal error \n");
	}	
	return(sigfunc);
}
