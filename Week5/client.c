#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // Kiểm tra số lượng tham số dòng lệnh
    if (argc < 3) {
        printf("Sai cú pháp. Sử dụng: ./client IPAddress PortNumber\n");
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
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Vòng lặp nhập dữ liệu từ người dùng
    while (1) {
        // Hiển thị thông báo cho người dùng
        printf("Nhập username và password: ");

        // Nhập dữ liệu từ người dùng
        char username[BUFFER_SIZE], password[BUFFER_SIZE];
        scanf("%s %s", username, password);

        // Gửi dữ liệu cho server
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "%s %s", username, password);
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Nhận phản hồi từ server
        char response[BUFFER_SIZE];
        int received_bytes = recvfrom(sockfd, response, BUFFER_SIZE, 0, NULL, NULL);
        if (received_bytes < 0) {
            perror("Không thể nhận dữ liệu từ server");
            return 1;
        }

            // Xử lý phản hồi
        if (strcmp(response, "OK") == 0) {
            // Đăng nhập thành công
            printf("Đăng nhập thành công\n\n");

            // Gửi yêu cầu đổi mật khẩu
            while (1) {
                // Hiển thị thông báo cho người dùng
                printf("Bạn có muốn đổi mật khẩu không (y/n): ");

                // Nhập dữ liệu từ người dùng
                char input;
                scanf("%c", &input);

                // Xử lý phản hồi
                if (input == 'y') {
                    // Nhập mật khẩu mới
                    printf("Nhập mật khẩu mới: ");
                    scanf("%s", password);

                    // Gửi mật khẩu mới cho server
                    sprintf(buffer, "%s", password);
                    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

                    // Nhận phản hồi từ server
                    received_bytes = recvfrom(sockfd, response, BUFFER_SIZE, 0, NULL, NULL);
                    if (received_bytes < 0) {
                        perror("Không thể nhận dữ liệu từ server");
                        return 1;
                    }

                    // Xử lý phản hồi
                    if (strcmp(response, "OK") == 0) {
                        // Đổi mật khẩu thành công
                        printf("Đổi mật khẩu thành công\n\n");
                    } else {
                        // Đổi mật khẩu thất bại
                        printf("Đổi mật khẩu thất bại\n\n");
                    }
                } else if (input == 'n') {
                // Không đổi mật khẩu
                break;
                } else {
                    // Nhập sai
                    printf("Nhập sai. Vui lòng nhập lại\n");
                }
            }
        } else if (strcmp(response, "INVALID PASSWORD") == 0) {
            // Mật khẩu sai
            printf("Mật khẩu sai\n");
        } else {
            // Lỗi không xác định
            printf("Lỗi không xác định\n");
        }

        // Hiển thị thông báo cho người dùng
        printf("Bạn có muốn tiếp tục không (y/n): ");

        // Nhập dữ liệu từ người dùng
        char input;
        scanf("%c", &input);

        // Xử lý phản hồi
        if (input == 'y') {
            continue;
        } else {
            break;
        }
    }

    // Đóng socket
    close(sockfd);

    return 0;
}

