#include "common.h"

void print_client_message(const char *prefix, const char *message, const char *color)
{
    printf("\033[1;31mclient\033[0m: [%s %s \033[0m] ==> %s\n", color, prefix, message);
}

void print_server_message(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("\033[1;31mserver\033[0m: ==> ");
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

int send_tcp_package(int socket_fd, CmdType cmd, const char *arg1, const char *arg2, const char *data, int data_length)
{
    TCPpackage package;
    memset(&package, 0, sizeof(TCPpackage));

    package.header.cmd = cmd;
    if (arg1 != NULL)
    {
        strncpy(package.header.arg1, arg1, sizeof(package.header.arg1) - 1);
    }
    if (arg2 != NULL)
    {
        strncpy(package.header.arg2, arg2, sizeof(package.header.arg2) - 1);
    }
    package.header.data_length = data_length;
    if (data != NULL && data_length > 0)
    {
        memcpy(package.data, data, data_length);
    }

    // 先发送头部信息
    int header_size = sizeof(TCPHeader);
    int bytes_sent = send(socket_fd, &package.header, header_size, 0);
    if (bytes_sent == -1)
    {
        perror("send header");
        return -1;
    }

    // 发送数据部分
    if (data_length > 0)
    {
        bytes_sent = send(socket_fd, package.data, data_length, 0);
        if (bytes_sent == -1)
        {
            perror("send data");
            return -1;
        }
    }

    return bytes_sent;
}

int receive_tcp_package(int socket_fd, TCPpackage *package)
{
    memset(package, 0, sizeof(TCPpackage));

    // 先接收头部信息
    int header_size = sizeof(TCPHeader);
    int bytes_received = recv(socket_fd, &package->header, header_size, 0);
    if (bytes_received == -1)
    {
        perror("recv header");
        return -1;
    }

    // 根据头部信息中的数据长度接收数据部分
    int data_length = package->header.data_length;
    if (data_length > 0)
    {
        bytes_received = recv(socket_fd, package->data, data_length, 0);
        if (bytes_received == -1)
        {
            perror("recv data");
            return -1;
        }
    }

    return bytes_received;
}

int send_file(int socket_fd, const char *local_file_name, const char *remote_file_name, bool resume, long fileoffset)
{
    FILE *file = fopen(local_file_name, "rb");
    if (file == NULL)
    {
        perror("fopen");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    long offset = 0;
    if (resume)
    {
        offset = fileoffset;
        printf("Resuming from offset %ld...\n", offset);
        fseek(file, offset, SEEK_SET);
    }

    size_t remaining_size = file_size - offset;
    while (remaining_size > 0)
    {
        char buffer[1024];
        size_t bytes_to_read = (remaining_size > sizeof(buffer)) ? sizeof(buffer) : remaining_size;
        size_t bytes_read = fread(buffer, 1, bytes_to_read, file);
        if (bytes_read < bytes_to_read && ferror(file))
        {
            perror("fread");
            fclose(file);
            return -1;
        }
        if (send_tcp_package(socket_fd, CMD_TYPE_PUT, remote_file_name, NULL, buffer, bytes_read) == -1)
        {
            fclose(file);
            return -1;
        }
        remaining_size -= bytes_read;
    }

    // 发送一个空的数据包表示传输结束
    if (send_tcp_package(socket_fd, CMD_TYPE_PUT, remote_file_name, NULL, NULL, 0) == -1)
    {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int receive_file(int socket_fd, const char *remote_file_name, const char *local_file_name, bool resume, long fileoffset)
{
    (void)remote_file_name;
    FILE *file = NULL;
    long offset = 0;

    if (resume)
    {
        file = fopen(local_file_name, "ab");
        if (file == NULL)
        {
            perror("fopen");
            return -1;
        }
        offset = fileoffset;
        fseek(file, offset, SEEK_END);
        printf("Resuming from offset %ld...\n", offset);
    }
    else
    {
        file = fopen(local_file_name, "wb");
        if (file == NULL)
        {
            perror("fopen");
            return -1;
        }
    }

    while (1)
    {
        TCPpackage package;
        if (receive_tcp_package(socket_fd, &package) == -1)
        {
            fclose(file);
            return -1;
        }
        if (package.header.data_length == 0)
        {
            // 收到空包，表示传输结束
            break;
        }
        size_t bytes_written = fwrite(package.data, 1, package.header.data_length, file);
        printf("Received %ld bytes...\n", bytes_written);
        if (bytes_written < (size_t)package.header.data_length && ferror(file))
        {
            perror("fwrite");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}
