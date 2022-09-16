#include "server.h"


int main (int argc, char **argv) {
	size_t ncomps;
	int serv_port;
	int client_port;

	if (argc != 4) {
		printf ("Usage: ./server _server port_ _client port_ _num of computers_ \n");
		exit (EXIT_FAILURE);
	}

	serv_port = atoi (argv[1]);
	client_port = atoi (argv[2]);
	sscanf (argv[3], "%zu", &ncomps);

	if ((ncomps < 1) | (serv_port < 1)) {
		printf ("Usage: ./server _server port_ _client port_ _num of computers_ \n");
		exit (EXIT_FAILURE);
	}

	ServerInit (serv_port, ncomps, client_port);

	exit (EXIT_SUCCESS);
}