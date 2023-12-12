#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/md5.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <ctype.h>

#define PORT 5550   /* Port that will be opened */ 
#define BUF_SIZE 1024
#define MMD5 DIGEST LENGTH 16

int main(int argc, char *argv[]) {
    int listen_sock, conn_sock;
    char recv_data[BUF_SIZE];
    int bytes_sent, bytes_received;
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sin_size;
    int port;
    regex_t regex;

    // Check if the command line parameter is specified.
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 0;
    }

    // Get the port number from the command line parameter.
    port = atoi(argv[1]);

    // Construct a TCP socket to listen connection request.
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("\nError: ");
        return 0;
    }

    // Bind address to socket.
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("\nError: ");
        return 0;
    }

    // Listen request from client.
    if (listen(listen_sock, BACKLOG) == -1) {
        perror("\nError: ");
        return 0;
    }

    // Compile the regular expression to match `.jpg` or `.png` extensions.
    if (regcomp(&regex, "^.*?\\.JPG$|^.*?\\.PNG$", 0) != 0) {
        printf("Failed to compile regular expression.\n");
        return 0;
    }

    // Communicate with client.
    while (1) {
        // Accept request.
        sin_size = sizeof(struct sockaddr_in);
        if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1) {
            perror("\nError: ");
            return 0;
        }

        printf("You got a connection from %s\n", inet_ntoa(client.sin_addr));

        // Communicate with client
        while (1) {
            // Check if client has sent data.
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(conn_sock, &readfds);
            int select_result = select(conn_sock + 1, &readfds, NULL, NULL, NULL);
            if (select_result < 0) {
                perror("\nError: ");
                close(conn_sock);
                return 0;
            } else if (select_result == 0) {
                // Client has not sent data yet.
                continue;
            }

            // Receive message from client.
            bytes_received = recv(conn_sock, recv_data, BUF_SIZE - 1, 0); // blocking
            if (bytes_received <= 0) {
                printf("\nConnection closed");
                close(conn_sock);
                return 0;
            } else {
                recv_data[bytes_received] = '\0';
                printf("\nReceive: %s ", recv_data);

                // Check if the received string contains only letters and digits.
                int valid = 1;
                extern int isalnum(int c);
                for (int i = 0; i < bytes_received; i++) {
                    if (!isalnum(recv_data[i])) {
                        valid = 0;
                        break;
                    }
                }

                // If the received string is not valid, send back an error message.
                if (!valid) {
                    send(conn_sock, "Invalid string", 14, 0);
                    continue;
                }

                // If the received string is an image file, display the content of the
                // file.

                // Check if the received string matches the regular expression.
                int match = regexec(&regex, recv_data, 0, NULL, 0);
                if (match == 0) {
                    // Read image file content.
                    FILE *f = fopen(recv_data, "rb");
                    if (f == NULL) {
                        // Send an error message if the image file is not found.
                        send(conn_sock, "File not found", strlen("File not found"), 0);
                        continue;
                    }

                    // Send image file content to client.
                    while (!feof(f)) {
                        int n = fread(recv_data, 1, BUF_SIZE, f);
                        if (n < 0) {
                            // Send an error message if the image file cannot be read.
                            send(conn_sock, "Cannot open file", strlen("Cannot open file"), 0);
                            break;
                        }
                        send(conn_sock, recv_data, n, 0);
                    }

                    fclose(f);
                } else {
                    // Encode the received string.
                    unsigned char md5_digest[MD5_DIGEST_LENGTH];
                    MD5((unsigned char *)recv_data, bytes_received, md5_digest);

                    // Split the MD5 digest into two parts.
                    char part1[MD5_DIGEST_LENGTH / 2];
                    char part2[MD5_DIGEST_LENGTH / 2];
                    for (int i = 0; i < MD5_DIGEST_LENGTH / 2; i++) {
                        part1[i] = md5_digest[i];
                        part2[i] = md5_digest[MD5_DIGEST_LENGTH / 2 + i];
                    }

                    // Display the two parts of the MD5 digest
                    printf("Part 1: %s\n", part1);
                    printf("Part 2: %s\n", part2);

                    // Send the two parts to the client.
                    send(conn_sock, part1, MD5_DIGEST_LENGTH / 2, 0);
                    send(conn_sock, part2, MD5_DIGEST_LENGTH / 2, 0);
                }
            }

        } // End conversation.
        close(conn_sock);
    }

    close(listen_sock);
    return 0;
}
