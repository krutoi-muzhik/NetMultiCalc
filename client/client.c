#include "client.h"

double calc (int nthreads, double lower, double upper, double step) {
	if (upper < lower) {
		printf ("Invalid limits: upper should be larger than lower\n");;
		exit (EXIT_FAILURE);
	}
}