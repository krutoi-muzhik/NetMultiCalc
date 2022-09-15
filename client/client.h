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

enum Keep {
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

	double (* CountFunc) (double);
} compmem_t;

typedef struct {
	double upper;
	double lower;
	double step;

	int core_id;
} threadmem_t;

double Calc (int nthreads, double lower, double upper, double step);

#endif