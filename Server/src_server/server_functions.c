#include "server.h"
#include <dirent.h> // for opendir, readdir, closedir

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
