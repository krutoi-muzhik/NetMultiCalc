#include "client.h"

double Calc (compmem_t *cmem) {
	if (cmem->upper < cmem->lower) {
		printf ("Invalid limits: upper should be larger than lower\n");;
		exit (EXIT_FAILURE);
	}


}

void ClientInit (int client_port, int nthreads) {
	int serv_port = 0;

	struct sockaddr_in addr;
	memset (&addr, '0', sizeof (addr));
	soocklen_t addr_len = sizeof (addr);

	Broadcast (client_port, &serv_port, &addr, &addr_len);

}

void Broadcast (int client_port, int *serv_port, struct sockaddr_in *addr, socklen_t *addr_len) {
	int err = 0;
	struct sockaddr_in local_addr;
	int bcsock;
	int optval = 1;		// need > 0 to set socket flag

	bcsock = socket (AF_INET, SOCK_DGRAM, 0);
	if (bcsock < 0) {
		perror ("bcsock socket");
		goto bc_error;
	}

	memset (&local_addr, '0', sizeof (local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons (client_port);
	local_addr.sin_addr.s_addr = htonl (INADDR_ANY);

	err = setsockopt (bcsock, SOL_SOCKET, SO_REUSEADDR, &k, sizeof (int));
	if (!err) {
		perror ("bcsock setsockopt");
		goto bc_error;
	}

	printf ("waiting for server\n");

	err = recvfrom (bcsock, serv_port, sizeof (int), 0, (struct sockaddr*) addr, addr_len);
	if (err != sizeof (int)) {
		perror ("recvfrom serv_port");
		goto bc_error;
	}

	close (bcsock);
	return;

bc_error:
	close (bcsock);
	exit (EXIT_FAILURE);
}


int ClientTCP (int *serv_port, struct sockaddr_in *addr, int nthreads) {
	int err = 0;
	int sock_data;
	int keep_cnt = KEEPCNT;
	int keep_idle = KEEPIDLE;
	int keep_intvl = KEEPINTVL;
	struct sockaddr_in serv_addr;
	socklen_t serv_addr_len;

	sockdata = socket (AF_INET, SOCK_STREAM, 0);

	err = setsockopt (sock_data, IPPROTO_TCP, TCP_KEEPCNT, &keep_cnt, sizeof (int));
	if (!err) {
		perror ("setsockopt keep_cnt");
		goto TCP_client_error;
	}
	err = setsockopt (sock_data, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof (int));
	if (!err) {
		perror ("setsockopt keep_idle");
		goto TCP_client_error;
	}
	err = setsockopt (sock_data, IPPROTO_TCP, TCP_KEEPINTVL, &keep_intvl, sizeof (int));
	if (!err) {
		perror ("setsockopt keep_intvl");
		goto TCP_client_error;
	}

	memset (&serv_addre, '0', sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (*serv_port);
	serv_addr.sin_addr = addr->sin_addr;

	serv_addr_len = sizeof (serv_addr);
	printf ("serv_addr = %s\n", inet_ntoa (serv_addr.sin_addr));

	err = connect (sock_data, (struct sockaddr*) &serv_addr, serv_addr_len);
	if (err < 0)
		perror ("sock_data connect");

	printf ("connected\n");

	return sock_data;

TCP_client_error:
	close (sock_data);
	exit (EXIT_FAILURE);
}

double ClientCalc (int sock, int nthreads) {
	int err = 0;
	compmem_t recv_mem;
	double sum;

	err = send (sock, &nthreads, sizeof (int), MSG_NOSIGNAL);
	if (err < 0) {
		perror ("send nthreads");
		goto client_calc_error;
	}

	memset (recv_mem, '0', sizeof (recv_mem));
	err = recv (sock, &recv_mem, sizeof (recv_mem));
	if (err < 0) {
		perror ("recv compmem");
		goto client_calc_error;
	}

	printf ("ncomp = %d \nupper = %lf \nlower = %lf \nstep = %lf\n", 
			recv_mem->ncomp, recv_mem->upper, 
			recv_mem->lower, recv_mem->step);

	recv_mem.nthreads = nthreads;
	sum = Calc (&recv_mem);
	printf ("subsum = %lf\n", sum);

	err = send (sock, &sum, sizeof (double), MSG_NOSIGNAL);
	if (err < 0) {
		perror ("send subsum");
		goto client_calc_error;
	}

	close (sock);
	return sum;

client_calc_error:
	close (sock);
	exit (EXIT_FAILURE);
}

double Calc (compmem_t *comp_mem) {

}