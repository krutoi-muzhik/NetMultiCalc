# Description

This programm calculates a definite integral, but it's aim is to make the calculation parallel on several computers, using sockets. After that on each machine it calculates parallel on several CPU cores, using threads.

## Server-Client communication



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

