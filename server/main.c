#include "server.h"


int main (int argc, char **argv) {
	size_t ncomps;
	int serv_port;

	if (argc != 3) {
		printf ("Usage: ./server _server port_ _client port_ _num of computers_ \n");
		exit (EXIT_FAILURE);
	}

	serv_port = atoi (argv[1]);
	sscanf (argv[2], "%zu", &ncomps);

	if ((ncomps < 1) | (serv_port < 1)) {
		printf ("Usage: ./server _server port_ _client port_ _num of computers_ \n");
		exit (EXIT_FAILURE);
	}

	ServerInit (serv_port, ncomps);

	exit (EXIT_SUCCESS);
}