#include "client.h"

void ClientInit (int client_port, int nthreads) {
	int serv_port = 0;
	int sock_data;
	struct sockaddr_in addr;
	memset (&addr, '0', sizeof (addr));
	socklen_t addr_len = sizeof (addr);

	CatchBroadcast (client_port, &serv_port, &addr, &addr_len);
	sock_data = ClientTCP (&serv_port, &addr, nthreads);
	ClientCalc (sock_data, nthreads);

	return;
}

void CatchBroadcast (int client_port, int *serv_port, struct sockaddr_in *addr, socklen_t *addr_len) {
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

	err = setsockopt (bcsock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (int));
	if (err) {
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

	sock_data = socket (AF_INET, SOCK_STREAM, 0);

	err = setsockopt (sock_data, IPPROTO_TCP, TCP_KEEPCNT, &keep_cnt, sizeof (int));
	if (err) {
		perror ("setsockopt keep_cnt");
		goto TCP_client_error;
	}
	err = setsockopt (sock_data, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof (int));
	if (err) {
		perror ("setsockopt keep_idle");
		goto TCP_client_error;
	}
	err = setsockopt (sock_data, IPPROTO_TCP, TCP_KEEPINTVL, &keep_intvl, sizeof (int));
	if (err) {
		perror ("setsockopt keep_intvl");
		goto TCP_client_error;
	}

	memset (&serv_addr, '0', sizeof (serv_addr));
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

	memset (&recv_mem, '0', sizeof (recv_mem));
	err = recv (sock, &recv_mem, sizeof (recv_mem), 0);
	if (err < 0) {
		perror ("recv compmem");
		goto client_calc_error;
	}

	printf ("ncomp = %d \nupper = %lf \nlower = %lf \nstep = %lf\n", 
			recv_mem.ncomp, recv_mem.upper, 
			recv_mem.lower, recv_mem.step);

	recv_mem.nthreads = nthreads;
	sum = Calculate (&recv_mem);
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

void *Integral (void *data) {
	int err;
	cpu_set_t cpuset;
	threadmem_t *mem = (threadmem_t *) data;

	CPU_ZERO (&cpuset);
	CPU_SET (mem->core_id, &cpuset);
	err = pthread_setaffinity_np (mem->thread_id, sizeof (cpuset), &cpuset);
	if (err) {
		printf ("Error %d in core %d\n", err, mem->core_id);
		perror ("CPU_ALLOC");
	}

	mem->sum = 0;
	for (double x = mem->lower; x < mem->upper; x += mem->step) {
		mem->sum += Count (x) * mem->step;
	}
	
	return data;
}

double Calculate (compmem_t *comp_mem) {
	int err;
	int nprocs;
	char *thread_mem;
	threadmem_t *mem;
	int mem_size;
	double intvl;
	double sum;

	nprocs = get_nprocs ();
	if (comp_mem->nthreads > nprocs) 
		comp_mem->nthreads = nprocs;

	mem_size = (sizeof (threadmem_t) / PAGE_SZ + 1) * PAGE_SZ;
	thread_mem = (char*) calloc (comp_mem->nthreads, mem_size);

	intvl = (comp_mem->upper - comp_mem->lower) / nprocs;
	for (size_t i = 0; i < nprocs; i ++) {
		mem = (threadmem_t *) (thread_mem + mem_size * i);
		mem->lower = comp_mem->lower + intvl * i;
		mem->upper = mem->lower + intvl;
		mem->step = comp_mem->step;
		mem->core_id = i;

		err = pthread_create (&(mem->thread_id), NULL, Integral, mem);
		if (err)
			perror ("pthread_create id");
	}

	for (size_t i = 0; i < nprocs; i ++) {
		mem = (threadmem_t *) (thread_mem + mem_size * i);
		err = pthread_join (mem->thread_id, NULL);
		sum += mem->sum;
	}

	return sum;
}

double Count (double x) {
	return sin (cos (x)) - 5 * x * x + 10 * exp (x * x) + x;
}