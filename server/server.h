#ifndef SERVER_H_

#define SERVER_H_

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

typedef struct {
	double upper;
	double lower;
	double step;

	size_t ncomp;
	int nthreads;

	double (* CountFunc) (double);
} compmem_t;

double GetResult (int ncomps, int socket, double (*CountFunc) (double));



#endif