#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h> // for va_list, va_start, va_end

// 使用前缀避免命名冲突
typedef enum
{
    CMD_TYPE_LS,         // 列出目录
    CMD_TYPE_CD,         // 改变目录
    CMD_TYPE_GET,        // 获取文件
    CMD_TYPE_PUT,        // 上传文件
    CMD_TYPE_RNM,        // 重命名
    CMD_TYPE_RMV,        // 删除文件
    CMD_TYPE_RESP,       // 响应
    CMD_TYPE_RESUME_GET, // 断点续传
    CMD_TYPE_RESUME_PUT, // 断点续传
    CMD_TYPE_EXIT,       // 退出
    CMD_TYPE_CHECK       // 检查文件是否存在
} CmdType;

typedef struct
{
    CmdType cmd;       // 命令类型
    char arg1[32];     // 命令参数1
    char arg2[32];     // 命令参数2
    int total_packets; // 如果是文件传输，这是分包数量
    int packet_number; // 如果是文件传输，这是当前包编号
    int seq_number;    // 序列号
    int ack_flag;      // 确认标志
    int data_length;   // 数据长度
} TCPHeader;

typedef struct
{
    TCPHeader header;
    char data[1024]; // 数据，如果是文件传输，这是文件数据；如果是响应，这是响应信息
} TCPpackage;

/**
 * @brief 打印客户端消息
 *
 * @param prefix 消息前缀
 * @param message 消息内容
 * @param color 消息颜色
 */
void print_client_message(const char *prefix, const char *message, const char *color);

/**
 * @brief 打印服务器消息
 *
 * @param format 格式化字符串
 * @param ... 可变参数列表
 */
void print_server_message(const char *format, ...);

/**
 * @brief 发送TCP包
 *
 * @param socket_fd 套接字文件描述符
 * @param cmd 命令类型
 * @param arg1 命令参数1
 * @param arg2 命令参数2
 * @param data 数据
 * @param data_length 数据长度
 * @return int 发送结果，成功返回发送的字节数，失败返回-1
 */
int send_tcp_package(int socket_fd, CmdType cmd, const char *arg1, const char *arg2, const char *data, int data_length);

/**
 * @brief 接收TCP包
 *
 * @param socket_fd 套接字文件描述符
 * @param package 接收的TCP包
 * @return int 接收结果，成功返回接收的字节数，失败返回-1
 */
int receive_tcp_package(int socket_fd, TCPpackage *package);

/**
 * @brief 发送文件到服务器
 *
 * @param socket_fd 套接字文件描述符
 * @param local_file_name 本地文件名
 * @param remote_file_name 远程文件名
 * @param resume 是否启用断点续传
 * @param offset 文件偏移量
 * @return int 发送结果，成功返回0，失败返回-1
 */
int send_file(int socket_fd, const char *local_file_name, const char *remote_file_name, bool resume, long fileoffset);

/**
 * @brief 从服务器接收文件
 *
 * @param socket_fd 套接字文件描述符
 * @param remote_file_name 远程文件名
 * @param local_file_name 本地文件名
 * @param resume 是否启用断点续传
 * @param fileoffset 文件偏移量
 * @return int 接收结果，成功返回0，失败返回-1
 */
int receive_file(int socket_fd, const char *remote_file_name, const char *local_file_name, bool resume, long fileoffset);

#endif // COMMON_H
