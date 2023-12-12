/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFF_SIZE 1024

int hostNameToIp(char *, char *);
int ipToHostName(char *, struct in_addr, char *);


//
int main(int argc, char* argv[]) {
    int sockfd;
    char buff[BUFF_SIZE];
    struct sockaddr_in server_addr, clident_addr;
    int bytes_received,sin_size;

    if(argc != 2) {
        printf("\nError: ");
        exit(EXIT_FAILURE);
    }
    printf("\nSuccessfull create socket.");

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    //
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    //
    if(bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    char result[1000];
    struct in_addr ip;
    int x;
    while(1) {
        memset(result,'\0',1000);
        bytes_received = recvfrom(sockfd, (char *)buff, BUFF_SIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &sin_size);
        buff[bytes_received] = '\0';
        printf("\tReceive: \"%s\" from client %s:%d\n", buff, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if(!inet_pton(AF_INET,buff,&ip)) {
            x = hostNameToIp(buff, result);
        } else x = ipToHostName(buff, ip,result);
        if(x == 1) strcpy(buff,"Error");
        sendto(sockfd, (char *)result, strlen(result), MSG_CONFIRM, (const struct sockaddr *) &client_addr, sizeof(client_addr));
        printf("\nMessage was sent to client...");
    }
    char *md5_hash = md5_hash(result);
	strcpy(result, md5_hash);
    
    return 0;
}

int hostNameToIp(char *param, char *message) {
    struct hostent *he;
    struct in_addr **addr_list;

    if((he = gethostbyname(param)) == NULL) {
        herror("gethostbyname");
        return 1;
    }
    addr_list = (struct in_addr **)he->h_addr_list;
        strcpy(message, "Official name: ");
        strcat(message, inet_ntoa(*addr_list[0]));
        strcat(message, "\nAlias name: \n");
        for(int i = 1; addr_list[i] != NULL; i++) {
            strcat(message,inet_ntoa(*addr_list[i]));
            strcat(message, "\n");
        }
    return 0;
}

int ipToHostName(char *param, struct in_addr ip, char *message) {
    struct hostent *hp;
    if((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL) {
        herror("gethostbyaddr");
        return 1;
    }
    //
        strcpy(message,"Official name: ");
        strcat(message, hp->h_name);
        strcat(message, "\nAlias name: \n");
        for(int i = 0; hp->h_aliases[i]!=NULL; i++) {
            strcat(message, hp->h_aliases[i]);
            strcat(message,"\n");
        }
        return 0;
}

char *md5_hash(char *str) {
    unsigned char md5_digest[MD5_DIGEST_LENGTH];
    MD5_CTX md5_ctx;

    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, str, strlen(str));
    MD5_Final(md5_digest, &md5_ctx);

    char *md5_hash_str = malloc(33); // 32 hexadecimal characters + 1 null terminator
    if (md5_hash_str == NULL) {
        perror("malloc");
        exit(1);
    }

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(md5_hash_str + i * 2, "%02x", md5_digest[i]);
    }

    return md5_hash_str;
}