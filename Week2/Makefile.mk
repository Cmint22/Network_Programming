CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g

all: account_management

account_management: account_management.o
	$(CC) $(CFLAGS) -o account_management account_management.o

account_management.o: account_management.c
	$(CC) $(CFLAGS) -c account_management.c

clean:
	rm -f account_management account_management.o