#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // Kiểm tra số lượng tham số dòng lệnh
    if (argc < 2) {
        printf("Sai cú pháp. Sử dụng: ./server PortNumber\n");
        return 1;
    }

    // Mở socket UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Không thể mở socket");
        return 1;
    }

    // Cấu hình địa chỉ IP và số hiệu cổng của server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bắt đầu lắng nghe các kết nối từ client
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Không thể bind socket");
        return 1;
    }

    // Vòng lặp tiếp nhận yêu cầu từ client
    while (1) {
        // Tạo buffer để lưu trữ dữ liệu nhận được từ client
        char buffer[BUFFER_SIZE];

        // Nhận dữ liệu từ client
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (received_bytes < 0) {
            perror("Không thể nhận dữ liệu từ client");
            return 1;
        }

        // Xử lý yêu cầu từ client
        char username[BUFFER_SIZE], password[BUFFER_SIZE];
        sscanf(buffer, "%s %s", username, password);

        // Kiểm tra mật khẩu
        int is_login_success = check_password(username, password);

        // Gửi phản hồi cho client
        char response[BUFFER_SIZE];
        if (is_login_success) {
            strcpy(response, "OK");
        } else {
            strcpy(response, "NOT OK");
        }
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_addr_len);

        // Nếu đăng nhập thành công, có thể nhận yêu cầu đổi mật khẩu
        if (is_login_success) {
            // Nhận dữ liệu từ client
            received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
            if (received_bytes < 0) {
                perror("Không thể nhận dữ liệu từ client");
                return 1;
            }

            // Xử lý yêu cầu đổi mật khẩu
            char new_password[BUFFER_SIZE];
            sscanf(buffer, "%s", new_password);

            // Kiểm tra mật khẩu mới
            int is_password_valid = check_password_format(new_password);

            // Gửi phản hồi cho client
            if (is_password_valid) {
                // Mật khẩu mới được mã hóa bằng SHA256
                char sha256_password[BUFFER_SIZE];
                sha256(new_password, strlen(new_password), sha256_password);

                // Gửi mật khẩu mới cho client
                response = sha256_password;
                sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_addr_len);
            } else {
                response = "INVALID PASSWORD";
                sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, client_addr_len);
                }

            // Nếu nhận được yêu cầu đăng xuất
            if (strcmp(buffer, "bye") == 0) {
                break;
            }
        }

        // Đóng socket
        close(sockfd);

        return 0;
    }
}

// Hàm kiểm tra mật khẩu
int check_password(char *username, char *password) {
    // Mở file account.txt
    FILE *fp = fopen("account.txt", "r");
    if (fp == NULL) {
        return 0;
    }

    // Kiểm tra số lần nhập sai mật khẩu
    static int count = 0;
    count++;

    // Block account nếu nhập sai mật khẩu 3 lần
    if (count == 3) {
        // Ghi nhận trạng thái account là bị block
        FILE *fp = fopen("account.txt", "r+");
        if (fp == NULL) {
        return 0;
        }

        char line[BUFFER_SIZE];
        while (!feof(fp)) {
        fgets(line, BUFFER_SIZE, fp);

        // Tìm kiếm dòng chứa username
        char *token = strtok(line, " ");
        if (strcmp(token, username) == 0) {
            // Thay đổi trạng thái account thành bị block
            token = strtok(NULL, " ");
            strcat(token, " 0");
            fseek(fp, -strlen(line), SEEK_CUR);
            fputs(line, fp);
            break;
        }
        }

        fclose(fp);
        return 0;
    }

    // Duyệt qua file account.txt
    while (!feof(fp)) {
        // Đọc một dòng từ file
        char line[BUFFER_SIZE];
        fgets(line, BUFFER_SIZE, fp);

        // Phân tách dòng thành username và password
        char *token = strtok(line, " ");
        char *username_in_file = token;
        token = strtok(NULL, " ");
        char *password_in_file = token;

        // So sánh username và password
        if (strcmp(username, username_in_file) == 0 && strcmp(password, password_in_file) == 0) {
            // Mật khẩu chính xác
            fclose(fp);
            return 1;
        }
    }

    // Mật khẩu không chính xác
    fclose(fp);
    return 0;
}

// Hàm kiểm tra định dạng mật khẩu
int check_password_format(char *password) {
    // Mật khẩu phải chứa ít nhất 8 ký tự
    if (strlen(password) < 8) {
        return 0;
    }

    // Mật khẩu phải chứa ít nhất một ký tự số và một ký tự chữ cái
    for (int i = 0; i < strlen(password); i++) {
        if (!isdigit(password[i]) && !isalpha(password[i])) {
            return 0;
        }
    }

    // Mật khẩu hợp lệ
    return 1;
}
