# Description

This programm calculates a definite integral, but it's aim is to make the calculation parallel on several computers, using sockets. After that on each machine it calculates parallel on several CPU cores, using threads.

## Server-Client communication

To pass data between server and clients, clients must know the servers's address, so first of all the server starts sharing his address on a broadcast. You can see it's implementation in function Sendbroadcast in ```server/server.c```
```C
void SendBroadcast (int client_port, int serv_port)
```

After the client has got server's address, he starts TCP connection and sends to the server the number of threads he wants to use.

After calculating the integral limits for every computer (based on their number of threads), server sends information about the integral (limits and step) to every client. Here he also starts his "stopwatch"

After getting the subsums from the clients back, server counts the final sum and the ammount of time, taken to get the answer.

# Client Calculation

After getting the information about the task, the client starts his calculation.

It uses ```nprocs()``` to get information of the max number of threads available on the machine.

It uses the special struct to store information for calculating on each core.

```C
typedef struct {
	double upper;
	double lower;
	double step;

	double sum;

	int core_id;
	pthread_t thread_id;
} threadmem_t;
```

We also match memory by the pagesize (64 bytes), to make different cores caches not intersect.

```C
	mem_size = (sizeof (threadmem_t) / PAGE_SZ + 1) * PAGE_SZ;
	thread_mem = (char*) calloc (comp_mem->nthreads, mem_size);
```

# Usage 

To check the result of computing, u should have packages make and gcc.

## Make

Use ```make``` or ```make all``` to compile all parts of programm.

To compile server or client separately, use ```make cl``` or ```make sv```

Use ```make clean``` to autoremove executable, objective and dependencies files.

## Run

To run a whole project, tou should

1. Run several clients in different terminals, using the same port numbers
2. Only after all clients are ready run a server, telling him the port of clients

### Server

To run server use ```./sv SV_PORT CL_PORT N_CLIENTS```

For example ```./sv 1234 2000 2```'

### Client

To run a client use ```./cl CL_PORT N_THREADS```

For example ```./cl 2000 8```

## Note

Make sure, that:

1. Client port is the same in clients and server arguements.
2. You start server ONLY AFTER all clients are already in run.

