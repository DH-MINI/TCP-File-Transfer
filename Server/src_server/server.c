#include "server.h"
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SERVER_PORT 8080
#define BACKLOG 10 // 连接请求队列的最大长度

int server_socket;

/**
 * @brief 信号处理函数，用于优雅地关闭服务器
 *
 * @param sig 信号
 */
void handle_sigint(int sig)
{
    printf("\nShutting down server...\n");
    if (server_socket != -1)
    {
        close(server_socket);
    }
    exit(0);
}

/**
 * @brief 初始化服务器套接字
 *
 * @return int 成功返回套接字描述符，失败退出程序
 */
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

/**
 * @brief 处理客户端连接
 *
 * @param client_socket 客户端套接字描述符
 */
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
            handle_resume_put_command(client_socket, package.header.arg1);
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

int main()
{
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int client_socket;

    // 注册信号处理函数
    signal(SIGINT, handle_sigint);

    // 设置服务器
    server_socket = setup_server();
    print_server_message("Server setup complete, waiting for connections...");

    while (1)
    {
        sleep(2);
        sin_size = sizeof(struct sockaddr_in);
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }

        print_server_message("Received connection from %s", inet_ntoa(client_addr.sin_addr));

        // 创建子进程处理客户端连接
        if (!fork())
        {
            // 子进程
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        }

        // 父进程
        close(client_socket);
    }

    return 0;
}
