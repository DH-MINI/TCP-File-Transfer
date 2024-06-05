#ifndef CLIENT_H
#define CLIENT_H

#include "../../Common/common.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

typedef enum
{
    MODE_LOCAL,
    MODE_REMOTE
} ClientMode;

/**
 * @brief 初始化客户端套接字
 *
 * @return int 成功返回套接字描述符，失败退出程序
 */
int setup_client();

/**
 * @brief 检查传入的命令和参数是否有效
 *
 * @param command 命令
 * @param arg1 参数1
 * @param arg2 参数2
 * @return true 参数有效
 * @return false 参数无效
 */
bool check_args(const char *command, const char *arg1, const char *arg2);

/**
 * @brief 处理用户输入的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param command 用户输入的命令
 * @param arg1 命令参数1
 * @param arg2 命令参数2
 * @param mode 客户端模式 (Local 或 Remote)
 */
void handle_command(int client_socket, const char *command, const char *arg1, const char *arg2, ClientMode mode);

/**
 * @brief 读取服务器响应
 *
 * @param client_socket 客户端套接字描述符
 */
void receive_response(int client_socket);

/**
 * @brief 打印帮助信息
 */
void print_help();

/**
 * @brief 打印欢迎信息
 */
void print_welcome_banner();

// Local command handlers
/**
 * @brief 处理本地列出目录的命令
 *
 * @param dir 目录路径
 */
void handle_local_ls(const char *dir);

/**
 * @brief 处理本地改变目录的命令
 *
 * @param dir 目录路径
 */
void handle_local_cd(const char *dir);

/**
 * @brief 检查文件是否存在
 *
 * @param filename 文件名
 * @return int 存在返回1，不存在返回0
 */
int check_file_exists(const char *filename);

/**
 * @brief 提示用户对已存在的文件进行操作
 *
 * @param filename 文件名
 * @param choice 用户选择
 */
void prompt_file_action(char *filename, char *choice);

/**
 * @brief 处理本地重命名文件的命令
 *
 * @param old_name 旧文件名
 * @param new_name 新文件名
 */
void handle_local_rename(const char *old_name, const char *new_name);

/**
 * @brief 处理本地删除文件的命令
 *
 * @param file_name 文件名
 */
void handle_local_remove(const char *file_name);

/**
 * @brief 处理本地检查文件是否存在的命令
 *
 * @param file_name 文件名
 */
void handle_local_check(const char *file_name);

/**
 * @brief 处理断点续传获取文件的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param remote_file_name 远程文件名
 * @param local_file_name 本地文件名
 */
void handle_remote_resume_get(int client_socket, const char *remote_file_name, const char *local_file_name);

/**
 * @brief 处理断点续传上传文件的命令
 *
 * @param client_socket 客户端套接字描述符
 * @param local_file_name 本地文件名
 * @param remote_file_name 远程文件名
 */
void handle_remote_resume_put(int client_socket, const char *local_file_name, const char *remote_file_name);

#endif // CLIENT_H
