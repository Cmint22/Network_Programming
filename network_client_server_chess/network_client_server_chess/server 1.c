#include <stdio.h>		// printf
#include <string.h>             // strlen
#include <stdlib.h>             // strlen
#include <sys/socket.h>		// socket
#include <arpa/inet.h>          // inet_addr
#include <unistd.h>             // write
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
    system("clear");
    struct sockaddr_in server, client;
    int socket_desc, client_sock, c;

    // Tao socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) { printf("Could not create socket"); }
    puts("Socket created");

    // Tao struct sockaddr_in
    server.sin_family = AF_INET;			// IPv4
    server.sin_addr.s_addr = INADDR_ANY;		
    server.sin_port = htons( atoi(argv[1]) );	

    // Bind
    if( bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0) {
	perror("bind failed. Error");			
	return 1;
    }
    puts("bind done");
    listen(socket_desc , 3);

    // Doi ket noi tu client
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    // Chap nhan ket noi tu client
    client_sock = accept (socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0) {
	perror("accept failed");
	return 1;
    }
    puts("Connection accepted");
    system("clear");
    printf("\n######################### CHESS #########################\n");  
    printBoard (board, pieces_gamer1, pieces_gamer2); // Ve ban co
    return 0;
}
