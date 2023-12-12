CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: UDPclient2 UDPserver2

client: UDPclient2.c
	${CC} UDPclient2.c  -o client2

server: UDPserver2.c
	${CC} UDPserver2.c  -o server2

clean:
	rm -f *.o *~