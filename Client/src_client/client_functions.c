#include "client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

int setup_client()
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
    if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        exit(1);
    }

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    print_client_message("RESPONSE", "Connected to the server successfully", "\033[1;34m");

    return sockfd;
}

bool check_args(const char *command, const char *arg1, const char *arg2)
{
    if (command == NULL)
    {
        return false;
    }

    // print strlen() 检查参数长度 方便下面检验
    // printf("strlen(command): %ld\n", strlen(command));
    // printf("strlen(arg1): %ld\n", strlen(arg1));
    // printf("strlen(arg2): %ld\n", strlen(arg2));

    // 检查命令和参数的有效性
    if (strcmp(command, "cd") == 0 || strcmp(command, "mode") == 0 || strcmp(command, "rmv") == 0 || strcmp(command, "check") == 0)
    {
        return strlen(arg1) != 0 && strlen(arg2) == 0;
    }
    else if (strcmp(command, "get") == 0 || strcmp(command, "put") == 0 ||
             strcmp(command, "rnm") == 0 || strcmp(command, "rget") == 0 ||
             strcmp(command, "rput") == 0)
    {
        return strlen(arg1) != 0 && strlen(arg2) != 0;
    }
    else if (strcmp(command, "help") == 0 || strcmp(command, "exit") == 0)
    {

        return strlen(arg1) == 0 && strlen(arg2) == 0;
    }
    else if (strcmp(command, "ls") == 0)
    {
        return (strlen(arg1) == 0 && strlen(arg2) == 0 || strlen(arg1) != 0 && strlen(arg2) == 0);
    }

    return false;
}

void print_welcome_banner()
{
    printf("\n========================================\n");
    printf("     Welcome to the TCP File Transfer    \n");
    printf("========================================\n");
    printf("          Author: DH_MINI              \n");
    printf("========================================\n");
    printf("  Type 'help' for available commands.\n");
    printf("========================================\n\n");
}

void print_help()
{

    printf("\nAvailable commands:\n");
    printf("======================================= ======================================== \n");
    printf("ls\t [directory]\t\t\tList the contents of the specified directory on the server.\n");
    printf("cd\t [directory]\t\t\tChange the current directory on the server.\n");
    printf("get\t [remote_file]\t [local_file]\tDownload a file from the server.\n");
    printf("put\t [local_file]\t [remote_file]\tUpload a file to the server.\n");
    printf("rnm\t [old_name]\t [new_name]\tRename a file on the server.\n");
    printf("rmv\t [file_name]\t\t\tRemove a file from the server.\n");
    printf("check\t [file_name]\t\t\tCheck if a file exists on the server.\n");
    printf("rget\t [remote_file]\t [local_file]\tResume a file download from the server.\n");
    printf("rput\t [local_file]\t [remote_file]\tResume a file upload to the server.\n");
    printf("mode\t [local/remote]\t\t\tSwitch between local and remote mode.\n");
    printf("help\t\t\t\t\tDisplay this help information.\n");
    printf("exit\t\t\t\t\tExit the client program.\n");
    printf("======================================= ========================================\n\n");
}

void prompt_file_action(char *filename, char *choice)
{

    // printf("File '%s' already exists. Choose an action:\n", filename);
    print_client_message("INFO", "File already exists. Choose an action:", "\033[1;33m");
    printf("o - Overwrite\n");
    printf("s - Skip\n");
    printf("r - Rename\n");
    printf("Choice ==> ");
    fgets(choice, sizeof(choice), stdin);
    choice[strcspn(choice, "\n")] = 0;

    switch (choice[0])
    {
    case 'o':
        // Overwrite
        break;
    case 's':
        // Skip
        filename[0] = '\0'; // Clear filename to indicate skipping
        break;
    case 'r':
        // Rename
        printf("Enter new filename: ");
        char Newfilename[256];
        fgets(Newfilename, 256, stdin);
        Newfilename[strcspn(Newfilename, "\n")] = 0;

        if (strcmp(Newfilename, filename) == 0)
        {
            print_client_message("ERROR", "New filename is the same as the old filename. Canceling rename.", "\033[1;31m");
            filename[0] = '\0'; // Clear filename to indicate canceling
        }
        else
        {
            snprintf(filename, 256, "%s", Newfilename);
        }
        break;
    default:
        printf("Invalid choice. Skipping.\n");
        filename[0] = '\0'; // Clear filename to indicate skipping
        break;
    }
}

void receive_response(int client_socket)
{
    TCPpackage package;
    receive_tcp_package(client_socket, &package);
    print_client_message("RESPONSE", package.data, "\033[1;34m");
}

void handle_remote_command(int client_socket, const char *command, const char *arg1, const char *arg2)
{
    char modifiable_arg2[256];
    char choice[10];
    strncpy(modifiable_arg2, arg2, sizeof(modifiable_arg2));
    modifiable_arg2[sizeof(modifiable_arg2) - 1] = '\0';

    if (strcmp(command, "ls") == 0)
    {
        if (send_tcp_package(client_socket, CMD_TYPE_LS, arg1, NULL, NULL, 0) == -1)
        {
            print_client_message("ERROR", "Failed to send CMD_TYPE_LS", "\033[1;31m");
        }
        else
        {
            receive_response(client_socket);
        }
    }
    else if (strcmp(command, "cd") == 0)
    {
        if (send_tcp_package(client_socket, CMD_TYPE_CD, arg1, NULL, NULL, 0) == -1)
        {
            print_client_message("ERROR", "Failed to send CMD_TYPE_CD", "\033[1;31m");
        }
        else
        {
            receive_response(client_socket);
        }
    }
    else if (strcmp(command, "get") == 0)
    {
        while (check_file_exists(modifiable_arg2))
        {
            prompt_file_action(modifiable_arg2, choice);
            if (modifiable_arg2[0] == '\0')
            {
                print_client_message("INFO", "Get command skipped", "\033[1;33m");
                return;
            }
            if (choice[0] == 'o')
            {
                break;
            }
        }

        if (send_tcp_package(client_socket, CMD_TYPE_CHECK, arg1, NULL, NULL, 0) == -1)
        {
            print_client_message("ERROR", "Failed to send CMD_TYPE_CHECK", "\033[1;31m");
            return;
        }
        else
        {
            TCPpackage package;
            receive_tcp_package(client_socket, &package);
            if (strncmp(package.data, "File does not exist:", 19) == 0)
            {
                print_client_message("ERROR", "File does not exist on the server", "\033[1;31m");
                return;
            }
        }

        if (send_tcp_package(client_socket, CMD_TYPE_GET, arg1, modifiable_arg2, NULL, 0) == -1)
        {
            print_client_message("ERROR", "Failed to send CMD_TYPE_GET", "\033[1;31m");
            return;
        }
        else
        {
            receive_file(client_socket, arg1, modifiable_arg2, false, 0); // 默认不使用断点续传
        }
    }
    else if (strcmp(command, "put") == 0)
    {
        print_client_message("ERROR", "PUT command is only available in LOCAL mode", "\033[1;31m");
    }
    else if (strcmp(command, "rnm") == 0)
    {
        if (send_tcp_package(client_socket, CMD_TYPE_RNM, arg1, arg2, NULL, 0) == -1)
        {
            print_client_message("ERROR", "Failed to send CMD_TYPE_RNM", "\033[1;31m");
        }
        else
        {
            receive_response(client_socket);
        }
    }
    else if (strcmp(command, "rmv") == 0)
    {
        if (send_tcp_package(client_socket, CMD_TYPE_RMV, arg1, NULL, NULL, 0) == -1)
        {
            print_client_message("ERROR", "Failed to send CMD_TYPE_RMV", "\033[1;31m");
        }
        else
        {
            receive_response(client_socket);
        }
    }
    else if (strcmp(command, "check") == 0)
    {
        if (send_tcp_package(client_socket, CMD_TYPE_CHECK, arg1, NULL, NULL, 0) == -1)
        {
            print_client_message("ERROR", "Failed to send CMD_TYPE_CHECK", "\033[1;31m");
        }
        else
        {
            receive_response(client_socket);
        }
    }
    else if (strcmp(command, "rget") == 0)
    {
        handle_remote_resume_get(client_socket, arg1, arg2);
    }
    else if (strcmp(command, "rput") == 0)
    {
        print_client_message("ERROR", "RESUME PUT command is only available in LOCAL mode", "\033[1;31m");
    }
    else
    {
        print_client_message("ERROR", "Unknown command", "\033[1;31m");
    }
}

void handle_local_command(int client_socket, const char *command, const char *arg1, const char *arg2)
{
    if (strcmp(command, "ls") == 0)
    {
        handle_local_ls(arg1);
    }
    else if (strcmp(command, "cd") == 0)
    {
        handle_local_cd(arg1);
    }
    else if (strcmp(command, "get") == 0)
    {
        print_client_message("ERROR", "GET command is only available in REMOTE mode", "\033[1;31m");
    }
    else if (strcmp(command, "put") == 0)
    {
        if (check_file_exists(arg1) == false)
        {
            print_client_message("ERROR", "Local file does not exist", "\033[1;31m");
            return;
        }

        char choice[10];
        char modifiable_arg2[256];
        strncpy(modifiable_arg2, arg2, sizeof(modifiable_arg2));
        modifiable_arg2[sizeof(modifiable_arg2) - 1] = '\0';

        // 检查远程文件是否存在（需要先发送CHECK命令到服务器）
        while (true)
        {
            if (send_tcp_package(client_socket, CMD_TYPE_CHECK, modifiable_arg2, NULL, NULL, 0) == -1)
            {
                print_client_message("ERROR", "Failed to send CMD_TYPE_CHECK", "\033[1;31m");
                break;
            }
            else
            {
                TCPpackage package;
                receive_tcp_package(client_socket, &package);
                if (strncmp(package.data, "File does not exist:", 19) == 0 || choice[0] == 'o')
                {
                    // 上传文件
                    if (send_tcp_package(client_socket, CMD_TYPE_PUT, modifiable_arg2, NULL, NULL, 0) == -1)
                    {
                        print_client_message("ERROR", "Failed to send CMD_TYPE_PUT", "\033[1;31m");
                        return;
                    }
                    else
                    {
                        send_file(client_socket, arg1, modifiable_arg2, false, 0); // 默认不使用断点续传
                    }
                    TCPpackage package;
                    receive_tcp_package(client_socket, &package);
                    print_client_message("RESPONSE", package.data, "\033[1;34m");
                    return;
                }
                else
                {
                    prompt_file_action(modifiable_arg2, choice);
                    if (modifiable_arg2[0] == '\0')
                    {
                        print_client_message("INFO", "Put command skipped", "\033[1;33m");
                        break;
                    }
                }
            }
        }
    }
    else if (strcmp(command, "rnm") == 0)
    {
        handle_local_rename(arg1, arg2);
    }
    else if (strcmp(command, "rmv") == 0)
    {
        handle_local_remove(arg1);
    }
    else if (strcmp(command, "check") == 0)
    {
        handle_local_check(arg1);
    }
    else if (strcmp(command, "rget") == 0)
    {
        print_client_message("ERROR", "RESUME GET command is only available in REMOTE mode", "\033[1;31m");
    }
    else if (strcmp(command, "rput") == 0)
    {
        handle_remote_resume_put(client_socket, arg1, arg2);
    }
    else
    {
        print_client_message("ERROR", "Unknown command", "\033[1;31m");
    }
}

void handle_command(int client_socket, const char *command, const char *arg1, const char *arg2, ClientMode mode)
{
    if (mode == MODE_REMOTE)
    {
        handle_remote_command(client_socket, command, arg1, arg2);
    }
    else
    {
        handle_local_command(client_socket, command, arg1, arg2);
    }
}

void handle_local_ls(const char *dir)
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
        perror("opendir");
        return;
    }

    while ((dir_entry = readdir(d)) != NULL)
    {
        snprintf(response + strlen(response), sizeof(response) - strlen(response), "%s\n", dir_entry->d_name);
    }

    print_client_message("RESPONSE", response, "\033[1;31m");
    closedir(d);
}

void handle_local_cd(const char *dir)
{
    char response[1024] = {0};

    if (chdir(dir) == -1)
    {
        print_client_message("RESPONSE", "Failed to change directory", "\033[1;31m");
    }
    else
    {
        snprintf(response, sizeof(response), "Successfully changed directory to: %s", dir);
        print_client_message("RESPONSE", response, "\033[1;32m");
    }
}

int check_file_exists(const char *filename)
{
    return access(filename, F_OK) != -1;
}

void handle_local_rename(const char *old_name, const char *new_name)
{
    if (rename(old_name, new_name) == -1)
    {
        print_client_message("ERROR", "Failed to rename file", "\033[1;31m");
    }
    else
    {
        print_client_message("RESPONSE", "Successfully renamed file", "\033[1;32m");
    }
}

void handle_local_remove(const char *file_name)
{
    if (remove(file_name) == -1)
    {
        print_client_message("ERROR", "Failed to remove file", "\033[1;31m");
    }
    else
    {
        print_client_message("RESPONSE", "Successfully removed file", "\033[1;32m");
    }
}

void handle_local_check(const char *file_name)
{
    char response[1024] = {0};

    if (check_file_exists(file_name))
    {
        snprintf(response, sizeof(response), "File exists: %s", file_name);
    }
    else
    {
        snprintf(response, sizeof(response), "File does not exist: %s", file_name);
    }

    print_client_message("RESPONSE", response, "\033[1;34m");
}

void handle_remote_resume_get(int client_socket, const char *remote_file_name, const char *local_file_name)
{
    long offset = 0;
    if (check_file_exists(local_file_name))
    {
        FILE *file = fopen(local_file_name, "rb");
        if (file)
        {
            fseek(file, 0, SEEK_END);
            offset = ftell(file);
            fclose(file);
        }
    }
    else
    {
        print_client_message("INFO", "Local file does not exist. Starting from the beginning...", "\033[1;33m");
    }

    if (send_tcp_package(client_socket, CMD_TYPE_RESUME_GET, remote_file_name, NULL, NULL, offset) == -1)
    {
        print_client_message("ERROR", "Failed to send CMD_TYPE_RESUME_GET", "\033[1;31m");
    }
    else
    {
        receive_file(client_socket, remote_file_name, local_file_name, true, offset);
    }
}

void handle_remote_resume_put(int client_socket, const char *local_file_name, const char *remote_file_name)
{
    long offset = 0;
    TCPpackage package;

    if (send_tcp_package(client_socket, CMD_TYPE_CHECK, remote_file_name, NULL, NULL, 0) == -1)
    {
        print_client_message("ERROR", "Failed to send CMD_TYPE_CHECK", "\033[1;31m");
        return;
    }

    if (receive_tcp_package(client_socket, &package) != -1 && strncmp(package.data, "File does not exist", 19) != 0)
    {
        offset = atol(package.data);
    }

    if (send_tcp_package(client_socket, CMD_TYPE_RESUME_PUT, remote_file_name, NULL, NULL, offset) == -1)
    {
        print_client_message("ERROR", "Failed to send CMD_TYPE_RESUME_PUT", "\033[1;31m");
    }
    else
    {
        send_file(client_socket, local_file_name, remote_file_name, true, offset);
        receive_response(client_socket);
    }
}
