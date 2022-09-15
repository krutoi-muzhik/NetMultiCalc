#include "client.h"

int main (int arhc, char **argv) {
	int port;
	int nthreads;
	double sum;

	if (argc != 3) {
		printf ("Usage: ./client _socket_ _num of threads_ \n");
		exit (EXIT_FAILURE);
	}

	port = atoi (argv[1]);
	nthreads = atoi (argv[2]);

	if ((port < 1) | (nthreads < 1)) {
		printf ("Usage: ./client _socket_ _num of threads_ \n");
		exit (EXIT_FAILURE);
	}

	ClientInit (port, nthreads);

	exit (EXIT_SUCCESS);
}