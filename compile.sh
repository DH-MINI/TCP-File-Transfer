#!/bin/bash

# 检查是否安装了gcc
if which gcc >/dev/null 2>&1; then
    COMPILER="gcc"
elif which clang >/dev/null 2>&1; then
    COMPILER="clang"
else
    echo "Error: Neither gcc nor clang found. Please install either gcc or clang."

    exit 1
fi

# 编译公共部分
$COMPILER -c -o Common/common.o Common/common.c

# 编译客户端
$COMPILER -o Client/src_client/client Client/src_client/client.c Client/src_client/client_functions.c Common/common.o -lpthread

# 编译服务器端
$COMPILER -o Server/src_server/server Server/src_server/server.c Server/src_server/server_functions.c Common/common.o -lpthread

echo "Compilation finished."
