#ifndef SERVER_H
#define SERVER_H

#include "../../Common/common.h"

/**
 * @brief 初始化服务器套接字
 *
 * @return int 成功返回套接字描述符，失败退出程序
 */
int setup_server();

/**
 * @brief 处理客户端连接
 *
 * @param client_socket 客户端套接字描述符
 */
void handle_client(int client_socket);

/**
 * @brief 处理列出目录的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param dir 目录路径
 */
void handle_ls_command(int client_socket, const char *dir);

/**
 * @brief 处理改变目录的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param dir 目录路径
 */
void handle_cd_command(int client_socket, const char *dir);

/**
 * @brief 处理获取文件的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param remote_file_name 远程文件名
 * @param offset 文件偏移量
 */
void handle_get_command(int client_socket, const char *remote_file_name, long offset);

/**
 * @brief 处理上传文件的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param remote_file_name 远程文件名
 */
void handle_put_command(int client_socket, const char *remote_file_name);

/**
 * @brief 处理重命名文件的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param old_name 旧文件名
 * @param new_name 新文件名
 */
void handle_rnm_command(int client_socket, const char *old_name, const char *new_name);

/**
 * @brief 处理删除文件的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param file_name 文件名
 */
void handle_rmv_command(int client_socket, const char *file_name);

/**
 * @brief 处理检查文件是否存在的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param file_name 文件名
 */
void handle_check_command(int client_socket, const char *file_name);

/**
 * @brief 信号处理函数，用于优雅地关闭服务器
 *
 * @param sig 信号
 */
void handle_sigint(int sig);

/**
 * @brief 处理断点续传获取文件的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param remote_file_name 远程文件名
 * @param offset 文件偏移量
 */
void handle_resume_get_command(int client_socket, const char *remote_file_name, long offset);

/**
 * @brief 处理断点续传上传文件的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param remote_file_name 远程文件名
 */
void handle_resume_put_command(int client_socket, const char *remote_file_name);

#endif // SERVER_H
