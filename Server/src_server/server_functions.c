#include "server.h"
#include <dirent.h> // for opendir, readdir, closedir

void handle_sigint(int sig)
{
    (void)sig;
    printf("\nShutting down server...\n");
    if (server_socket != -1)
    {
        close(server_socket);
    }
    exit(0);
}

int setup_server()
{
    int sockfd;
    struct sockaddr_in server_addr;

    // 创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    // 绑定套接字
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    // 监听套接字
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    return sockfd;
}

void handle_client(int client_socket)
{
    TCPpackage package;
    while (1)
    {
        int bytes_received = receive_tcp_package(client_socket, &package);
        if (bytes_received == -1)
        {
            break;
        }

        switch (package.header.cmd)
        {
        case CMD_TYPE_LS:
            // 列出目录
            print_server_message("Received CMD_TYPE_LS");
            handle_ls_command(client_socket, package.header.arg1);
            break;
        case CMD_TYPE_CD:
            // 改变目录
            print_server_message("Received CMD_TYPE_CD");
            handle_cd_command(client_socket, package.header.arg1);
            break;
        case CMD_TYPE_GET:
            // 获取文件
            print_server_message("Received CMD_TYPE_GET");
            handle_get_command(client_socket, package.header.arg1, package.header.data_length);
            break;
        case CMD_TYPE_PUT:
            // 上传文件
            print_server_message("Received CMD_TYPE_PUT");
            handle_put_command(client_socket, package.header.arg1);
            break;
        case CMD_TYPE_RNM:
            // 重命名文件
            print_server_message("Received CMD_TYPE_RNM");
            handle_rnm_command(client_socket, package.header.arg1, package.header.arg2);
            break;
        case CMD_TYPE_RMV:
            // 删除文件
            print_server_message("Received CMD_TYPE_RMV");
            handle_rmv_command(client_socket, package.header.arg1);
            break;
        case CMD_TYPE_CHECK:
            // 检查文件是否存在（用于断点续传）
            print_server_message("Received CMD_TYPE_CHECK");
            handle_check_command(client_socket, package.header.arg1);
            break;
        case CMD_TYPE_RESUME_GET:
            // 断点续传获取文件
            print_server_message("Received CMD_TYPE_RESUME_GET");
            handle_resume_get_command(client_socket, package.header.arg1, package.header.data_length);
            break;
        case CMD_TYPE_RESUME_PUT:
            // 断点续传上传文件
            print_server_message("Received CMD_TYPE_RESUME_PUT");
            handle_resume_put_command(client_socket, package.header.arg1, package.header.data_length);
            break;
        case CMD_TYPE_EXIT:
            // 退出
            print_server_message("Received CMD_TYPE_EXIT");
            close(client_socket);
            exit(0);
            break;
        default:
            print_server_message("Received unknown command");
            break;
        }
    }

    close(client_socket);
}

void handle_ls_command(int client_socket, const char *dir)
{
    if (strlen(dir) == 0)
    {
        dir = ".";
    }
    DIR *d;
    struct dirent *dir_entry;
    char response[1024] = {0};

    d = opendir(dir);
    if (d == NULL)
    {
        snprintf(response, sizeof(response), "Failed to open directory: %s", dir);
        send_tcp_package(client_socket, CMD_TYPE_LS, NULL, NULL, response, strlen(response));
        return;
    }

    while ((dir_entry = readdir(d)) != NULL)
    {
        // 将每个目录项追加到响应缓冲区中
        strncat(response, dir_entry->d_name, sizeof(response) - strlen(response) - 1);
        strncat(response, "\n", sizeof(response) - strlen(response) - 1);
    }
    closedir(d);

    // 发送响应
    send_tcp_package(client_socket, CMD_TYPE_LS, NULL, NULL, response, strlen(response));
}

void handle_cd_command(int client_socket, const char *dir)
{
    char response[1024] = {0};

    // 尝试改变目录
    if (chdir(dir) == -1)
    {
        snprintf(response, sizeof(response), "Failed to change directory to: %s", dir);
    }
    else
    {
        snprintf(response, sizeof(response), "Successfully changed directory to: %s", dir);
    }

    // 发送响应回客户端
    send_tcp_package(client_socket, CMD_TYPE_CD, NULL, NULL, response, strlen(response));
}

void handle_get_command(int client_socket, const char *remote_file_name, long offset)
{
    (void)offset;
    send_file(client_socket, remote_file_name, remote_file_name, false, 0);
}

void handle_put_command(int client_socket, const char *remote_file_name)
{
    receive_file(client_socket, remote_file_name, remote_file_name, false, 0);

    char response[1024];
    snprintf(response, sizeof(response), "Server: Successfully received file: %s", remote_file_name);
    send_tcp_package(client_socket, CMD_TYPE_PUT, NULL, NULL, response, strlen(response));
}

void handle_rnm_command(int client_socket, const char *old_name, const char *new_name)
{
    char response[1024] = {0};

    if (rename(old_name, new_name) == -1)
    {
        snprintf(response, sizeof(response), "Failed to rename file from %s to %s", old_name, new_name);
    }
    else
    {
        snprintf(response, sizeof(response), "Successfully renamed file from %s to %s", old_name, new_name);
    }

    send_tcp_package(client_socket, CMD_TYPE_RNM, NULL, NULL, response, strlen(response));
}

void handle_rmv_command(int client_socket, const char *file_name)
{
    char response[1024] = {0};

    if (remove(file_name) == -1)
    {
        snprintf(response, sizeof(response), "Failed to remove file: %s", file_name);
    }
    else
    {
        snprintf(response, sizeof(response), "Successfully removed file: %s", file_name);
    }

    send_tcp_package(client_socket, CMD_TYPE_RMV, NULL, NULL, response, strlen(response));
}

void handle_check_command(int client_socket, const char *file_name)
{
    char response[1024] = {0};

    FILE *file = fopen(file_name, "rb");
    if (file == NULL)
    {
        snprintf(response, sizeof(response), "File does not exist: %s", file_name);
        send_tcp_package(client_socket, CMD_TYPE_CHECK, NULL, NULL, response, strlen(response));
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fclose(file);

    snprintf(response, sizeof(response), "%ld", file_size);
    send_tcp_package(client_socket, CMD_TYPE_CHECK, NULL, NULL, response, strlen(response));
}

void handle_resume_get_command(int client_socket, const char *remote_file_name, long offset)
{
    send_file(client_socket, remote_file_name, remote_file_name, true, offset);
}

void handle_resume_put_command(int client_socket, const char *remote_file_name, long offset)
{
    receive_file(client_socket, remote_file_name, remote_file_name, true, offset);

    char response[1024];
    snprintf(response, sizeof(response), "Successfully received file with resume: %s", remote_file_name);
    send_tcp_package(client_socket, CMD_TYPE_RESUME_PUT, NULL, NULL, response, strlen(response));
}
