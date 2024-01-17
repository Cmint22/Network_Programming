#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>

#define BUFF_SIZE 1024

#define BUFF_SIZE1 8192

#define FOLDER "image/"
int CheckFileFormat(char *filename)
{
    char *token = strtok(filename, ".");
    token = strtok(NULL, ".");
    if (strcmp(token, "bmp") == 0 || strcmp(token, "jpg") == 0 || strcmp(token, "png") == 0)
        return 1;
    return 0;
}
void receive_image(int socket, char *file_name)
{
    int buffersize = 0, recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;
    char imagearray[BUFF_SIZE];
    char verify = '1';
    FILE *image;
    char file_copy[100];
    strcpy(file_copy, file_name);
    // Kiem tra file
    if (!CheckFileFormat(file_copy))
    {
        printf("\nError: Wrong File Format!\n");
        return;
    }
    // Check file 
    char path_of_image[100];
    strcat(path_of_image, FOLDER);
    strcat(path_of_image, file_name);
    printf("%s\n", path_of_image);
    if (access(path_of_image, F_OK) != -1)
    {
        printf("\nFile Already Exists!\n");
        return;
    }
    stat = send(socket, file_name, BUFF_SIZE, 0);
    //Tim kich thuoc cua file
    stat = recv(socket, &size, sizeof(int), 0);
    // Size tra ve < 0 => Khong tim thay file
    if (size < 0)
    {
        printf("\nError: File Not Found!\n");
        return;
    }
    // printf("Packet received.\n");
    // printf("Packet size: %i\n", stat);
    // printf("Image size: %i\n", size);
    // printf(" \n");
    char buffer[] = "Got it";
    // Gui tin hieu xac nhan
    do
    {
        stat = send(socket, &buffer, sizeof(int), 0);
    } while (stat < 0);
    image = fopen(path_of_image, "w");
    if (image == NULL)
    {
        printf("Error has occurred. Image file could not be opened\n");
        return;
    }
    // Lap cho den khi nhan du du lieu cua file
    int need_exit = 0;
    struct timeval timeout = {10, 0};
    fd_set fds;
    int buffer_fd, buffer_out;
    while (recv_size < size)
    {
        // while(packet_index < 2){
        FD_ZERO(&fds);
        FD_SET(socket, &fds);
        buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
        if (buffer_fd < 0)
            printf("error: bad file descriptor set.\n");
        if (buffer_fd == 0)
            printf("error: buffer read timeout expired.\n");
        if (buffer_fd > 0)
        {
            do
            {
                read_size = recv(socket, imagearray, BUFF_SIZE + 1, 0);
            } while (read_size < 0);
            printf("Packet number received: %i\n", packet_index);
            printf("Packet size: %i\n", read_size);
            // Write the currently read data into our image file
            write_size = fwrite(imagearray, 1, read_size, image);
            printf("Written image size: %i\n", write_size);
            printf(" ");
             if (read_size != write_size)
            {
                printf("error in read write\n");
            }
            // Increment the total number of bytes read
            recv_size += read_size;
            packet_index++;
            printf("Total received image size: %i\n", recv_size);
            printf(" \n");
        }
    }
    fclose(image);
    printf("\nFile Transfer Is Completed…\n");
    return;
}

int main(int argc, char *argv[])
{
	int client_sock;
	char buff[BUFF_SIZE], connected[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent, bytes_received, sin_size;

	if (argc != 3)
	{
		perror("Error Input.\n");
		exit(0);
	}

	// Construct a TCP socket
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("\nError: ");
            exit(0);
        }

	// Define the address of the server
    bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[2]));
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	sin_size = sizeof(struct sockaddr);

	// Connect to server
          if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            {
                perror("Error: ");
                close(client_sock);
                exit(0);
            }
	bytes_received = recv(client_sock, connected, BUFF_SIZE - 1, 0);
	if (bytes_received < 0)
	{
		perror("Error: ");
		close(client_sock);
		return 0;
	}
	connected[bytes_received] = '\0';
	if (strcmp(connected, "done") == 0)
	{
		printf("\nUsername: ");
		memset(buff, '\0', (strlen(buff) + 1));
		fgets(buff, BUFF_SIZE, stdin);

		printf("Password: ");
		char password[BUFF_SIZE];
		memset(password, '\0', (strlen(password) + 1));
		fgets(password, BUFF_SIZE, stdin);
		strcat(buff, password);

		bytes_sent = send(client_sock, buff, strlen(buff), 0);
		if (bytes_sent < 0)
		{
			perror("Error: ");
			close(client_sock);
			return 0;
		}
	}

	// Step 4: Communicate with server
	while (strcmp(connected, "done") == 0)
	{
		// Received messsage form server
		bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
		if (bytes_received < 0)
		{
			perror("Error: ");
			close(client_sock);
			return 0;
		}
		buff[bytes_received] = '\0';
		if (strcmp(buff, "exit") == 0)
		{
			printf("Exit succesfull");
			return 0;
		}

		// Incorrect Password
		if (strcmp(buff, "incorrect_password") == 0)
		{
			printf("Incorrect password! Please enter other password: ");
			char password[BUFF_SIZE];
			memset(password, '\0', (strlen(password) + 1));
			fgets(password, BUFF_SIZE, stdin);

			// Gửi xâu `username\npassword` đến server
			bytes_sent = send(client_sock, password, strlen(password), 0);
			if (bytes_sent < 0)
			{
				perror("Error: ");
				close(client_sock);
				return 0;
			}
		}
        
		// Login thanh cong
		else if (strcmp(buff, "OK") == 0)
		{


			int key =0;
			 // Step 4: Communicate with server
				do{
					printf("\nMENU");
					printf("\n------------------------------");
					printf("\n1. Gui mot xau bat ky");
					printf("\n2. Gui noi dung mot file");
					printf("\n0. Exit ");
					printf("\nYour choice:\n");
				
				
					
					scanf("%d", &key);
					fflush(stdin);
					while (key != 1 && key != 2 && key != 0)
					{
						printf("Choice 1,2 or 0:\n");
						scanf("%d", &key);
						fflush(stdin);
					}
				

					if (key == 1)
					{
					// Send message
					char msg[BUFF_SIZE];
							fflush(stdin);
							printf("\nInsert string: ");

							scanf("%s",msg);
							
						
					}

					if (key == 2)
					{
						char file_name[BUFF_SIZE1];
						printf("Enter file path: ");
						scanf("%s",file_name);
						// fflush(stdin);
						// receive_image(client_sock,file_name);

					}

				}while(key != 0);

			
	
			//printf("Enter \"bye\" to exit): ");
			char buff[BUFF_SIZE];
			memset(buff, '\0', (strlen(buff) + 1));
			fgets(buff, BUFF_SIZE, stdin);
			bytes_sent = send(client_sock, buff, strlen(buff), 0);
		}
	
		else if (strcmp(buff, "Goodbye") == 0)
		{
			printf("Good bye!\n");
			exit(0);
		}
        else if (strcmp(buff, "notOK") == 0)
        {
            printf("Account is blocked!\n");
            exit(0);
        }
		else if (strcmp(buff, "error_digit") == 0)
		{
			printf("Error digit!\n");
			exit(0);
		}
		else if (strcmp(buff, "username_not_exist") == 0)
		{
			printf("Username not exist!\n");
			exit(0);
		}
		else if (strcmp(buff, "not_ready") == 0)
		{
			printf("not_ready!\n");
			exit(0);
		}
		else if (strcmp(buff, "exit") == 0)
		{
			printf("Exit succesfull\n");
			return 0;
		}
        else {
        buff[bytes_received] = '\0';

		printf("Reply from server: %s\n", buff);
        }
	}

	close(client_sock);
	return 0;
}
