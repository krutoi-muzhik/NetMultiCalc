CC = gcc
CFLAGS = -Wall -MMD
CLIBS = -lpthread -lm
CLIENT_OBJS = client/main.o client/client.o
SERVER_OBJS = server/main.o server/server.o


.PHONY:
	all

all: sv cl

sv: server/main.o server/server.o
	$(CC) $(CLIBS) $(CFLAGS) $^ -o $@

cl: client/main.o client/client.o
	$(CC) $(CFLAGS) $(CLIBS) $< -o $@

%.o: %.c %.h
	$(CC) $(CLIBS) $(CFLAGS) $< -c -o $@

server/main.o: server/main.c server/server.h
	$(CC) $(CFLAGS) $(CLIBS) $< -c -o $@

client/main.o: client/main.c client/client.h
	$(CC) $(CFLAGS) $(CLIBS) $< -c -o $@

-include *.d
