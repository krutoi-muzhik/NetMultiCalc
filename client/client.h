#ifndef CLIENT_H_

#define CLIENT_H_

#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

enum SIZES {
	PAGE_SZ = 64
};

enum CONSTS {
	LISTEN_BACKLOG = 100,
	ACCEPT_TIMEOUT_SEC = 0,
	ACCEPT_TIMEOUT_USEC = 1000,
	COMPUTE_TIMEOUT_SEC = 10000,
	COMPUTE_TIMEOUT_USEC = 0
};

enum KEEP {
	KEEPCNT = 3,
	KEEPIDLE = 2,
	KEEPINTVL = 1
};

typedef struct {
	double upper;
	double lower;
	double step;

	size_t ncomp;
	size_t nthreads;
} compmem_t;

typedef struct {
	double upper;
	double lower;
	double step;

	double sum;

	size_t core_id;
	pthread_t thread_id;
} threadmem_t;

void ClientInit (int client_port, size_t nthreads);
void CatchBroadcast (int client_port, int *serv_port, struct sockaddr_in *addr, socklen_t *addr_len);
int ClientTCP (int *serv_port, struct sockaddr_in *addr);
double ClientCalc (int sock, size_t nthreads);
void *Integral (void *data);
double Calculate (compmem_t *comp_mem);
double Count (double x);

#endif