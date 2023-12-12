CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: UDPclient UDPserver

client: UDPclient.c
	${CC} UDPclient.c  -o client

server: UDPserver.c
	${CC} UDPserver.c  -o server

clean:
	rm -f *.o *~