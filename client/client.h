#ifndef CLIENT_H_

#define CLIENT_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

enum SIZES {
	PAGE_SZ = 64
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

	int ncomp;
	int nthreads;
} compmem_t;

typedef struct {
	double upper;
	double lower;
	double step;

	double sum;

	int core_id;
	pthread_t thread_id;
} threadmem_t;

void ClientInit (int client_port, int nthreads);
void Broadcast (int client_port, int *serv_port, struct sockaddr_in *addr, socklen_t *addr_len);
int ClientTCP (int *serv_port, struct sockaddr_in *addr, int nthreads);
double ClientCalc (int sock, int nthreads);
void *Integral (void *data);
double Calculate (compmem_t *comp_mem);
double Count (double x);

#endif