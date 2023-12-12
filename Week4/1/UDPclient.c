/*UDP Echo Client*/
#include <stdio.h>			/* These are the usual header files */ 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define BUFF_SIZE 1024 

int main(int argc, char* argv[]) { 
	int sockfd; 
	char buff[BUFF_SIZE]; 
	struct sockaddr_in server_addr; 
	int bytes_received, sin_size;

	// Check parameter
	if(argc != 3) {
		printf("\nError: ");
		exit(EXIT_FAILURE);
	}
	// Construct a UDP socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
		perror("\nError: "); 
		exit(EXIT_FAILURE); 
	} 
	printf("\n[+]Client Socket Created Sucessfully.");
	
	// Define the address of the server
	memset(&server_addr, 0, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = inet_addr(argv[1]); 
	server_addr.sin_port = htons(atoi(argv[2])); 

	char number[50];
	// Communicate with server
	while (1){
		printf("\nInsert string to send:");
		memset(buff,'\0',(strlen(buff)+1));
		memset(number,'\0',(strlen(number)+1));
		fgets(buff, BUFF_SIZE, stdin);
		if(buff[0]=='\n') break;
		buff[strlen(buff)-1]='\0';
		sendto(sockfd, buff, strlen(buff), MSG_CONFIRM, (const struct sockaddr *) &server_addr, sizeof(server_addr)); 
		printf("\n[+]Successfully sent to server...\n"); 
			
		bytes_received = recvfrom(sockfd, (char *)buff, BUFF_SIZE, MSG_WAITALL, (struct sockaddr *) &server_addr, &sin_size); 
		buff[bytes_received] = '\0'; 
		bytes_received = recvfrom(sockfd, (char *)number, sizeof(number), MSG_WAITALL, (struct sockaddr *) &server_addr, &sin_size); 
		number[bytes_received] = '\0'; 
		printf("Reply from server: %s %s\n", buff, number);
	}
	close(client_sock); 
	return 0; 
} 