#include "client.h"

int main (int argc, char **argv) {
	int port;
	size_t nthreads;

	if (argc != 3) {
		printf ("Usage: ./client _port_ _num of threads_ \n");
		exit (EXIT_FAILURE);
	}

	port = atoi (argv[1]);
	sscanf (argv[2], "%zu", &nthreads);

	if ((port < 1) | (nthreads < 1)) {
		printf ("Usage: ./client _portsocket_ _num of threads_ \n");
		exit (EXIT_FAILURE);
	}

	while (1)
		ClientInit (port, nthreads);

	exit (EXIT_SUCCESS);
}