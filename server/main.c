#include "server.h"

int main (int argc, char **argv) {
	int socket;
	int ncomps;
	double sum;

	if (argc != 3)
		goto inval_args;

	sscanf (argv[1], "%d", &socket);
	sscanf (argv[2], "%d", &ncomps);

	if ((ncomps < 1) | (socket < 1))
		goto inval_args;

	sum = GetResult (ncomps, socket, -10f, 10f, 0.000001);
	printf ("Result: \t%lf", sum);

	exit (EXIT_SUCCESS);

inval_args:
	printf ("Usage: ./server _socket_ _num of computers_ \n");
	exit (EXIT_FAILURE);
}