CC = gcc
CHECK_FLAGS = -Werror -Wextra -Wnarrowing -Wstrict-prototypes -Wconversion
CFLAGS = -Wall -MMD $(CHECK_FLAGS)
CLIENT_LIBS = -lpthread -lm
CLIENT_OBJS = client/main.o client/client.o
SERVER_OBJS = server/main.o server/server.o


.PHONY:
	all

all: sv cl

sv: server/main.o server/server.o
	$(CC) $^ $(CFLAGS) -o $@

cl: client/main.o client/client.o
	$(CC) $^ $(CLIENT_LIBS) $(CFLAGS) -o $@

%.o: %.c %.h
	$(CC) $< $(CLIENT_LIBS) $(CFLAGS) -c -o $@

server/main.o: server/main.c server/server.h
	$(CC) $< $(CFLAGS) -c -o $@

client/main.o: client/main.c client/client.h
	$(CC) $< $(CLIENT_LIBS) $(CFLAGS) -c -o $@

clean:
	rm -f $(CLIENT_OBJS)
	rm -f $(SERVER_OBJS)

-include *.d
