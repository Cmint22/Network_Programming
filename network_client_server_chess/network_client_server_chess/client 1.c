#include <stdio.h>          // printf
#include <string.h>         // strcpy
#include <stdlib.h>         // atoi
#include <string.h>         // strlen
#include <sys/socket.h>     // socket
#include <arpa/inet.h>      // inet_addr
#include <unistd.h>         // for close
#include "communication.c"


// So quan co 2 ben
int pieces_gamer1 = 16;
int pieces_gamer2 = 16;



// BOARD
int board[8][8] =
{
    { 8, 7, 9,11,10, 9, 7, 8},
    {12,12,12,12,12,12,12,12},
    {13,13,13,13,13,13,13,13},
    {13,13,13,13,13,13,13,13},
    {13,13,13,13,13,13,13,13},
    {13,13,13,13,13,13,13,13},
    { 6, 6, 6, 6, 6, 6, 6, 6},
    { 2, 1, 3, 4, 5, 3, 1, 2}
};


int main(int argc , char *argv[]) {
    struct sockaddr_in server;
    int sock, r_connect;


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
	printf("ERRO: nao foi possivel criar o socket");
	return 1;
    }

    printf("# SOCKET CRIADO #\n");

    
    server.sin_family = AF_INET;				// IPv4
    server.sin_addr.s_addr = inet_addr(argv[1]); 		
    server.sin_port = htons(atoi(argv[2]));			
   
    r_connect = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (r_connect < 0) {
	printf("ERRO: conexao com o servidor falhou");
	return 2;
    }
    printf("# CONNECTADO #\n");

  
    
 	system("clear");
	printf("\n######################### CHESS #########################\n");

	printBoard (board, pieces_gamer1, pieces_gamer2);

	
    close(sock);
    return 0;
}
