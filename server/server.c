#include "server.h"

void ServerInit (int serv_port, int ncomps, int client_port) {
	int err;
	int sock_connect;
	int global_nthreads = 0;
	struct timeval timeout_accept;
	int param = 1;		// need > 0 for setsockopt
	int keep_cnt;
	int keep_idle;
	int keep_intvl;
	struct sockaddr_in serv_addr;
	int *sock_data;
	struct timeval time_begin, time_end;	

	struct sockaddr_in *client_addr;
	socklen_t client_addr_len = sizeof (struct sockaddr_in);
	size_t connected_clients;
	compmem_t *comp_mem = (compmem_t *) calloc (ncomps, sizeof (compmem_t));

	double sum;
	double current = 0.0;
	double upper = 10.0;
	double lower = -10.0;
	double step = 0.0000001;
	double intvl;

	timeout_accept.tv_sec = ACCEPT_TIMEOUT_SEC;
	timeout_accept.tv_usec = ACCEPT_TIMEOUT_USEC;

	sock_connect = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_connect < 0)
		perror("socket sock_connect");

	err = setsockopt (sock_connect, SOL_SOCKET, SO_REUSEADDR, &param, sizeof (int));
	if (err)
		perror ("setsockopt SO_REUSEADDR");
	err = setsockopt (sock_connect, SOL_SOCKET, SO_RCVTIMEO, &timeout_accept, sizeof (timeout_accept));
	if (err)
		perror ("setsockopt SO_RCVTIMEO");
	err = setsockopt (sock_connect, SOL_SOCKET, SO_KEEPALIVE, &param, sizeof (int));
	if (err)
		perror ("setsockopt SO_KEEPALIVE");

	keep_cnt = KEEPCNT;
	keep_idle = KEEPIDLE;
	keep_intvl = KEEPINTVL;

	err = setsockopt (sock_connect, IPPROTO_TCP, TCP_KEEPCNT, &keep_cnt, sizeof (int));
	if (err)
		perror ("setsockopt TCP_KEEPCNT");
	err = setsockopt (sock_connect, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof (int));
	if (err)
		perror ("setsockopt TCP_KEEPIDLE");
	err = setsockopt (sock_connect, IPPROTO_TCP, TCP_KEEPINTVL, &keep_intvl, sizeof (int));
	if (err)
		perror ("setsockopt TCP_KEEPINTVL");

	memset (&serv_addr, '0', sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	serv_addr.sin_port = htons (serv_port);

	err = bind (sock_connect, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
	if (err < 0)
		perror ("bind");

	err = listen (sock_connect, LISTEN_BACKLOG);
	if (err < 0)
		perror ("listen");

	SendBroadcast (client_port, serv_port);

	sock_data = (int *) calloc (ncomps, sizeof (int));
	client_addr = (struct sockaddr_in *) calloc (ncomps, sizeof (struct sockaddr_in));

	for (connected_clients = 0; connected_clients < ncomps; connected_clients ++) {
		sock_data[connected_clients] = accept (sock_connect, NULL, NULL);
		if (sock_data[connected_clients] < 0) {
			if (errno == EAGAIN) {
				printf ("Got all clients\n");
				break;
			} else
				perror ("accept client");
		}

		struct timeval compute_timeout;

		compute_timeout.tv_sec = COMPUTE_TIMEOUT_SEC;
		compute_timeout.tv_usec = COMPUTE_TIMEOUT_USEC;

		err = setsockopt (sock_data[connected_clients], SOL_SOCKET, SO_RCVTIMEO, &compute_timeout, sizeof (struct timeval));
		if (err)
			perror ("setsockopt sock_data SO_RCVTIMEO");

		client_addr[connected_clients].sin_family = AF_INET;
		client_addr[connected_clients].sin_port = htons (client_port);
	}

	if (connected_clients == 0) {
		printf ("No clients out there\n");
		goto error_serv;
	}

	printf ("%ld connected clients\n", connected_clients);

	gettimeofday(&time_begin, 0);

	for (size_t i = 0; i < connected_clients; i ++) {
		int ret = recv (sock_data[i], &(comp_mem[i].nthreads), sizeof (int), 0);
		if (ret < 0)
			perror ("recv nthreads");
		if (ret != sizeof (int)) {
			printf ("Client %ld disconnected\n", i);
			goto error_clients;
		}

		global_nthreads += comp_mem[i].nthreads;
		printf ("Client %ld wants to use %d threads\n", i, comp_mem[i].nthreads);
		comp_mem[i].ncomp = i;
	}

	printf ("Global %d threads\n", global_nthreads);
	intvl = (upper - lower) / global_nthreads;
	current = lower;

	for (size_t i = 0; i < ncomps; i ++) {
		comp_mem[i].step = step;
		comp_mem[i].lower = current;
		current += intvl * comp_mem[i].nthreads;
		comp_mem[i].upper = current;

		err = sendto (sock_data[i], &comp_mem[i], sizeof (comp_mem[i]), 0, (struct sockaddr *) (&client_addr[i]), client_addr_len);
		if (err != sizeof (comp_mem[i]))
			perror ("sendto comp_mem");
	}

	for (size_t i = 0; i < connected_clients; i ++) {
		printf ("client_addr = %s\n", inet_ntoa (client_addr[i].sin_addr));

		double res;
		int ret = recv (sock_data[i], &res, sizeof (double), 0);
		if (ret != sizeof (double)) {
			printf ("Client %ld disconnected\n", i);
			goto error_clients;
		}

		sum += res;
		printf ("Sub sum %ld: %lf\n", i, res);
	}

	gettimeofday(&time_end, 0);

	printf ("Resulting sum: %lf\n", sum);
	printf ("Taken time: %.3f seconds\n", time_end.tv_sec - time_begin.tv_sec + (time_end.tv_usec - time_begin.tv_usec) / 1000000.0);

error_clients:
	for (size_t i = 0; i < connected_clients; i ++) {
		close (sock_data[i]);
	}

error_serv:
	close (sock_connect);

	free (comp_mem);
	free (sock_data);
	free (client_addr);

	return;
}

void SendBroadcast (int client_port, int serv_port) {
	int err;
	int k = 1;
	int bcsock = 0;
	struct sockaddr_in bc_addr;
	int port = serv_port;

	bcsock = socket (AF_INET, SOCK_DGRAM, 0);
	if (bcsock < 0)
		perror ("socket bcsock");

	memset(&bc_addr, '0', sizeof(bc_addr));
	bc_addr.sin_family = AF_INET;
	bc_addr.sin_port = htons(client_port);
	bc_addr.sin_addr.s_addr = INADDR_BROADCAST;

	err = setsockopt(bcsock, SOL_SOCKET, SO_BROADCAST, &k, sizeof(int));
	if (err)
		perror("setsockopt sock broadcast");
	err = setsockopt(bcsock, SOL_SOCKET, SO_REUSEADDR, &k, sizeof(int));
	if (err)
		perror("setsockopt sock reuseaddres");

	err = bind (bcsock, (struct sockaddr *) &bc_addr, sizeof (bc_addr));
	if (err < 0)
		perror ("bind bcsock");


	err = sendto (bcsock, &port, sizeof (int), 0, (struct sockaddr *) &bc_addr, sizeof (bc_addr));
	if (err < 0)
		perror ("sendto bc");

	close (bcsock);

	return;
}