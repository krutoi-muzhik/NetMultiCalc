#include "server.h"

int main (int argc, char **argv) {
	int socket;
	int ncomps;
	double sum;

	if (argc != 3)
		goto inval_args;

	port = atoi (argv[1]);
	ncomps = atoi (argv[2]);

	if ((ncomps < 1) | (socket < 1))
		goto inval_args;

	printf ("Result: \t%lf", sum);

	exit (EXIT_SUCCESS);

inval_args:
	printf ("Usage: ./server _socket_ _num of computers_ \n");
	exit (EXIT_FAILURE);
}