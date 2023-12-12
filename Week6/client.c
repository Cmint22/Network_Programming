#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 8192

int main(int argc, char *argv[]) {
    int client_sock;
    char buff[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;

    // Check command line parameters
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    // Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("socket");
        exit(1);
    }

    // Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    // Step 3: Request to connet server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        perror("connect");
        exit(1);
    }

    // Step 4: Communicate with server
    while (1) {
        // Display menu
        printf("MENU\n-----------------------------------\n"
               "1. Gửi xâu bất kỳ\n"
               "2. Gửi nội dung một file\n"
               "Your choice: ");
        scanf("%d", &msg_len);

        // Handle user selections
        switch (msg_len) {
            case 1:
                // Send any string
                printf("Enter any string: ");
                if (feof(stdin)) {
                    break;
                }
                fgets(buff, BUFF_SIZE, stdin);
                bytes_sent = send(client_sock, buff, strlen(buff), 0);
                if (bytes_sent <= 0) {
                    if (ferror(client_sock)) {
                        perror("send");
                    }
                    printf("\nConnection closed!\n");
                    break;
                }

                // Receive response from server
                bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
                if (bytes_received <= 0) {
                    if (ferror(client_sock)) {
                        perror("recv");
                    }
                    printf("\nError!Cannot receive data from server!\n");
                    break;
                }

                buff[bytes_received] = '\0';
                printf("Reply from server: %s\n", buff);
                break;
            case 2:
                // Send file content
                printf("Enter file path: ");
                if (feof(stdin)) {
                    break;
                }
                fgets(buff, BUFF_SIZE, stdin);
                FILE *f = fopen(buff, "rb");
                if (f == NULL) {
                    printf("Cannot open file.\n");
                    break;
                }

                // Send file content to the server
                while ((bytes_sent = fread(buff, 1, BUFF_SIZE, f)) > 0) {
                    send(client_sock, buff, bytes_sent, 0);
                }
                fclose(f);

                // Receive response from server
                bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
                if (bytes_received <= 0) {
                    if (ferror(client_sock)) {
                        perror("recv");
                    }
                    printf("\nError!Cannot receive data from server!\n");
                    break;
                }

                buff[bytes_received] = '\0';
                printf("Reply from server: %s\n", buff);
                break;
            default:
                printf("Invalid selection.\n");
                break;
        }
    }

    // Step 5: Close socket
    close(client_sock);
    return 0;
}
