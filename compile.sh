#!/bin/bash

# 编译公共部分
clang -c -o Common/common.o Common/common.c

# 编译客户端
clang -o Client/src_client/client Client/src_client/client.c Client/src_client/client_functions.c Common/common.o -lpthread

# 编译服务器端
clang -o Server/src_server/server Server/src_server/server.c Server/src_server/server_functions.c Common/common.o -lpthread

echo "Compilation finished."
