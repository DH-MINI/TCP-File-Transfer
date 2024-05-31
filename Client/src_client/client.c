#include "client.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    int client_socket = setup_client();
    char command[256];
    char arg1[256];
    char arg2[256];
    char buffer[1024];
    ClientMode mode = MODE_REMOTE;

    print_welcome_banner();

    while (1)
    {
        // 清空命令和参数字符串
        memset(command, 0, sizeof(command));
        memset(arg1, 0, sizeof(arg1));
        memset(arg2, 0, sizeof(arg2));
        memset(buffer, 0, sizeof(buffer));

        printf("\033[1;31mclient\033[0m (%s): ==> ", (mode == MODE_REMOTE) ? "REMOTE" : "LOCAL");
        // 读取用户输入
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        int num_args = sscanf(buffer, "%s %s %s", command, arg1, arg2);
        // printf("num_args: %d\n", num_args);
        if (num_args == -1)
        {
            continue;
        }

        if (check_args(command, arg1, arg2) == false)
        {
            print_client_message("ERROR", "Invalid arguments.", "\033[1;31m");
            continue;
        }

        if (strcmp(command, "exit") == 0)
        {
            send_tcp_package(client_socket, CMD_TYPE_EXIT, NULL, NULL, NULL, 0);
            break;
        }
        else if (strcmp(command, "help") == 0)
        {
            print_help();
            continue;
        }
        else if (strcmp(command, "mode") == 0)
        {
            if (strcmp(arg1, "local") == 0)
            {
                mode = MODE_LOCAL;
            }
            else if (strcmp(arg1, "remote") == 0)
            {
                mode = MODE_REMOTE;
            }
            else
            {
                print_client_message("ERROR", "Invalid mode. Use 'local' or 'remote'.", "\033[1;31m");
            }
            continue;
        }

        handle_command(client_socket, command, arg1, arg2, mode);
    }

    close(client_socket);
    return 0;
}
