#include "client.h"

int main (int arhc, char **argv) {
	int port;
	int nthreads;
	double sum;

	if (argc != 3)
		goto inval_args;

	sscanf (argv[1], "%d", &port);
	sscanf (argv[2], "%d", &nthreads);
	if ((port < 1) | (nthreads < 1))
		goto inval_args;

	sum = Calc (nthreads, -10f, 10f, 0.000001);
	printf ("Result: \t%lf\n", sum);

	exit (EXIT_SUCCESS);

inval_args:
	printf ("Usage: ./client _socket_ _num of threads_ \n");
	exit (EXIT_FAILURE);

}