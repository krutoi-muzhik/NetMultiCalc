#ifndef SERVER_H_

#define SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>

enum KEEP {
	KEEPCNT = 3,
	KEEPIDLE = 2,
	KEEPINTVL = 1
};

typedef struct {
	double upper;
	double lower;
	double step;

	double sum;

	size_t ncomp;
	int nthreads;
} compmem_t;

enum CONSTS {
	LISTEN_BACKLOG = 100,
	ACCEPT_TIMEOUT_SEC = 0,
	ACCEPT_TIMEOUT_USEC = 1000,
	COMPUTE_TIMEOUT_SEC = 10000,
	COMPUTE_TIMEOUT_USEC = 0
};

void ServerInit (int serv_port, int ncomps, int client_port);
void SendBroadcast (int client_port, int serv_port);

#endif