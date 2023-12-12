/*UDP Echo Server*/
#include <stdio.h>			/* These are the usual header files */
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define BUFF_SIZE 1024 

int separateLetNum(char *, char *,char *);

int main(int argc, char* argv[]) { 
	int sockfd; 
	char buff[BUFF_SIZE]; 
	struct sockaddr_in server_addr, client_addr; 
	int bytes_received,sin_size;

	if(argc != 2){
		printf("\nError: ");
		exit(EXIT_FAILURE);
	}
	// Construct a UDP socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("\nSuccessful create socket.");
	
	memset(&server_addr, 0, sizeof(server_addr)); 
	memset(&client_addr, 0, sizeof(client_addr)); 
	
	// Bind address to socket
	server_addr.sin_family = AF_INET; // IPv4 
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	server_addr.sin_port = htons(atoi(argv[1])); 
	
	if(bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	char result[100];
	char number[50];
	// Communicate with clients
	while(1)
	{
		memset(result,'\0',100);
		memset(number,'\0',50);
		bytes_received = recvfrom(sockfd, (char *)buff, BUFF_SIZE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &sin_size); 
		buff[bytes_received] = '\0'; 
		printf("\tReceive: \"%s\" from client %s:%d\n", buff, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
		int x = separateLetNum(buff,result,number);
		if(x == 1){
			printf("\nError: ");
			strcpy(result,"\nError: Invalid character");
		}
		printf("\nServer reply: %s %s\n",buff,number);
		sendto(sockfd, (char *)result, strlen(result), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr)); 
		printf("\nMessage 1 was sent to client...."); 
		sendto(sockfd, (char *)number, strlen(number), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr)); 
		printf("\nMessage 2 was sent to client....\n"); 
	}
	return 0; 
} 

int separateLetNum(char *buff, char *result, char *number){
	int l1 = strlen(result);
	int l2 = strlen(number);
	for(int i = 0; i < strlen(buff); i++){
		int dec = (int)buff[i];
		if(dec >= 48 && dec <= 57) {
			*(number + l2) = buff[i];
			l2++;
		}else if((dec >= 65 && dec <= 90) || (dec >= 97 && dec <= 122)){
			*(result + l1) = buff[i];
			l1++;
		}else return 1;
	}
	if(l1 == 0 || l2 == 0) {
		return 1;
	}

	char *md5_hash = md5_hash(result);
	strcpy(result, md5_hash);

	return 0;
}

char* md5_hash(char *str) {
	unsigned char md5_digest[MD5_DIGEST_LENGTH];
	MD5_CTX md5_ctx;

	MD5_Init(&md5_ctx);
	MD5_Update(&md5_ctx, str, strlen(str));
	MD5_Final(md5_digest, &md5_ctx);

	char *md5_hash_str = malloc(33); 
	if (md5_hash_str == NULL) {
		perror("malloc");
		exit(1);
	}

	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(md5_hash_str + i * 2, "%02x", md5_digest[i]);
	}

	return md5_hash_str;
}
