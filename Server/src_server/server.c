#include "server.h"
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
