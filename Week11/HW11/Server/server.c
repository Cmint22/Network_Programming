#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>




#define MAX_SIZE 1024

typedef struct Message
{
    char digital[MAX_SIZE];
    char str[MAX_SIZE];
    bool checkValid;
} Message;

typedef struct Account
{
    char username[MAX_SIZE];
    char password[MAX_SIZE];
    int status;
    struct Account *next;
} Account;


pid_t pid;
pid_t wait (int *statloc);
pid_t waitpid (pid_t pid, int *statloc, int options);


void sig_chld(int signo)
{
pid_t pid;
int stat;
pid = waitpid(-1, &stat, WNOHANG );
printf("child %d terminated\n", pid);
}


void changeStatus(char *username_ck, int new_status);
void changePassword(char *username_ck, char *new_password);
char *convertMessage(char *buff);

int getStatus(char *username);
int validPassWord(char *username_ck, char *password_ck);

char line_break[] = "\n";

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sv_sock;
    char buff[MAX_SIZE];
    char otherPassword[MAX_SIZE], new_password[MAX_SIZE];
    int byte_send, byte_received_client, byte_received_client2, message_received_client, username_password_received_client,otherPassword_received_client;
    
    int sin_sz;
    

    if (argc != 2)
    {
        perror("Error Input.\n");
        exit(0);
    }

    
    if ((sv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error: ");
        exit(0);
    }

    
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server.sin_zero), 8);

    if (bind(sv_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("\nError");
        exit(0);
    }
    
    if (listen(sv_sock, 5) == -1) {
        perror("Error listening for connections");
        exit(0);
    }
 
    signal(SIGCHLD, sig_chld);
    
    while (1)
    {
        sin_sz = sizeof(struct sockaddr_in);
        
        int client_sock = accept(sv_sock, (struct sockaddr *)&client, &sin_sz) ; // Chấp nhận kết nối từ client
        
        if((pid = fork()) == 0) {

            close(sv_sock);
           if (client_sock < 0) {
               perror("\nError accepting connection");
               exit(0);
           }
        
        printf("Client connected from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        
        int bytes_sent = send(client_sock, "done", 4, 0);
        if (bytes_sent < 0)
            perror("\nError");
        
      
            while (1)
            {
                username_password_received_client = recv(client_sock, buff, MAX_SIZE - 1, 0);
                if (username_password_received_client < 0){
                    
                
                    perror("\nError :     ");
                break;
                }
                else
                {
                    buff[username_password_received_client - 1] = '\0';
                    char *newline_pos = strchr(buff, '\n');

                    if (newline_pos != NULL)
                    {
                        *newline_pos = '\0';

                        char username[MAX_SIZE];
                        strcpy(username, buff);
                        char *password = newline_pos + 1;
                        int count = 0;
                        

                        
                        if (validPassWord(username, password) == 2)
                        {
                            byte_send = send(client_sock, "username_not_exist", MAX_SIZE, 0);
                            exit(0);
                        }

                        
                        if (validPassWord(username, password) == 3)
                        {
                            byte_send = send(client_sock, "not_ready", MAX_SIZE, 0);
                            exit(0);
                        }

                    
                        while (validPassWord(username, password) == 0 && count < 2)
                        {
                            byte_send = send(client_sock, "incorrect_password", MAX_SIZE, 0);
                            if (byte_send < 0)
                                perror("\nError");
                            otherPassword_received_client = recv(client_sock, password, MAX_SIZE - 1, 0);
                            password[otherPassword_received_client - 1] = '\0';
                            count++;
                        }

                        if (count == 2)
                        {
                            byte_send = send(client_sock, "notOK", 6, 0);
                            changeStatus(username, 0);
                        }

                    
                        int status = getStatus(username);
                        if (status == 1)
                        {
                            byte_send = send(client_sock, "OK", MAX_SIZE, 0);
                            message_received_client = recv(client_sock, new_password, MAX_SIZE - 1, 0);
                            new_password[message_received_client - 1] = '\0';

                            // Signout
                            if (strcmp(new_password, "bye") == 0)
                            {
                                byte_send = send(client_sock, "Goodbye", MAX_SIZE, 0);
                                exit(0);
                            }
                            char *mess = convertMessage(new_password);

                            
                            if (strlen(mess) == 0)
                            {
                                byte_send = send(client_sock, "error_digital", MAX_SIZE, 0);
                                exit(0);
                            }
                            else // Change password
                            {
                                byte_send = send(client_sock, mess, strlen(mess), 0);
                                changePassword(username, new_password);
                                byte_send = send(client_sock, "exit", MAX_SIZE, 0);

                            }
                        }
                        else if (status == 0 || status == 2)
                        {
                            byte_send = send(client_sock, "account_not_ready", 18, 0);
                        }
                    }
                }
            }
            close(client_sock);
            exit(0);
        }
        

    close(client_sock);
    }
    return 0;
}

int getStatus(char *username_ck)
{
    char username[MAX_SIZE];
    char password[MAX_SIZE];
    int status;
    FILE *inp = fopen("account.txt", "r");
    do
    {
        if (fscanf(inp, "%s %s %d", username, password, &status) > 0)
        {
            if (!strcmp(username, username_ck))
            {
                return status;
            }
        }
    } while (1);
}



char *convertMessage(char *buff)
{
    char digital[MAX_SIZE];
    char str[MAX_SIZE];
    int digital_index = -1, str_index = -1;
    int length = strlen(buff);
    for (int i = 0; i < length; i++)
    {
        if (buff[i] <= '9' && buff[i] >= '0')
        {
            digital_index++;
            digital[digital_index] = buff[i];
        }
        else if ((buff[i] <= 'z' && buff[i] >= 'a') || (buff[i] <= 'Z' && buff[i] >= 'A'))
        {
            str_index++;
            str[str_index] = buff[i];
        }
        else
        {
            return "";
        }
    }
    digital[digital_index + 1] = '\0';
    str[str_index + 1] = '\0';
    char *tmp = strcat(digital, "\n");
    tmp = strcat(tmp, str);
    return tmp;
}


void changeStatus(char *username_ck, int new_status)
{
    const char *filename = "account.txt";
    FILE *file = fopen(filename, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        char username[MAX_SIZE];
        char password[MAX_SIZE];
        int status;

        if (sscanf(line, "%s %s %d", username, password, &status) == 3)
        {
            if (strcmp(username, username_ck) == 0)
            {
                status = new_status;
            }

            // rewrite tmp file
            fprintf(tempFile, "%s %s %d\n", username, password, status);
        }
    }

    fclose(file);
    fclose(tempFile);
    remove(filename);
    rename("temp.txt", filename);
    return;
}



int validPassWord(char *username_ck, char *password_ck)
{
    char username[MAX_SIZE];
    char password[MAX_SIZE];
    int status;
    FILE *inp = fopen("account.txt", "r");

    while (fscanf(inp, "%s %s %d", username, password, &status) > 0)
    {
        if (strcmp(username, username_ck) == 0)
        {
            if (strcmp(password, password_ck) == 0)
            {
                if (status != 1)
                {
                    return 3;
                }
                return 1;
            }
            else
                return 0;
        }
    }

    return 2;
    fclose(inp);
}
void changePassword(char *username_ck, char *new_password)
{
    const char *filename = "account.txt";
    FILE *file = fopen(filename, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        char username[MAX_SIZE];
        char password[MAX_SIZE];
        int status;
        if (sscanf(line, "%s %s %d", username, password, &status) == 3)
        {
            if (strcmp(username, username_ck) == 0)
            {
                strcpy(password, new_password);
            }

            // rewrite tmp file
            fprintf(tempFile, "%s %s %d\n", username, password, status);
        }
    }

    fclose(file);
    fclose(tempFile);
    remove(filename);
    rename("temp.txt", filename);
    return;
}
