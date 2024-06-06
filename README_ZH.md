# TCP 文件传输项目

## 概述

该项目实现了一个基于 TCP 的文件传输系统，包括客户端和服务器端。客户端可以向服务器发送各种命令以执行操作，如列出目录、更改目录、上传和下载文件以及恢复中断的传输。

## 项目结构

```
.
├── Client
│   ├── client_root
│   └── src_client
│       ├── client.c
│       ├── client.h
│       └── client_functions.c
├── Common
│   ├── common.c
│   └── common.h
├── Server
│   ├── server_root
│   └── src_server
│       ├── server.c
│       ├── server.h
│       └── server_functions.c
├── compile.sh
├── run_client.sh
└── run_server.sh
```

## 环境

- C 编译器（例如，GCC）
- 确保以下目录存在并具有必要的读/写权限：
  - `Client/client_root`
  - `Server/server_root`

## 编译

使用提供的 `compile.sh` 脚本编译客户端和服务器程序。

```sh
chmod +x compile.sh
./compile.sh
```

该脚本将编译源文件并生成客户端和服务器的可执行二进制文件。

## 运行服务器

要启动服务器，请使用 `run_server.sh` 脚本：

```sh
chmod +x run_server.sh
./run_server.sh
```

这将启动服务器，并开始监听客户端连接。

## 运行客户端

要启动客户端，请使用 `run_client.sh` 脚本：

```sh
chmod +x run_client.sh
./run_client.sh
```

客户端将连接到服务器，并提供一个交互式命令行界面，用于向服务器发送命令。

**提供的脚本使用 clang 进行编译。如果需要使用 gcc 编译，请修改脚本。**

## 命令

客户端支持以下命令：

- `ls [directory]`: 列出服务器上指定目录的内容。
- `cd [directory]`: 更改服务器上的当前目录。
- `get [remote_file] [local_file]`: 从服务器下载文件。
- `put [local_file] [remote_file]`: 向服务器上传文件。
- `rnm [old_name] [new_name]`: 重命名服务器上的文件。
- `rmv [file_name]`: 删除服务器上的文件。
- `check [file_name]`: 检查服务器上是否存在文件。
- `rget [remote_file] [local_file]`: 从服务器恢复文件下载。
- `rput [local_file] [remote_file]`: 向服务器恢复文件上传。
- `help`: 显示帮助信息。
- `exit`: 退出客户端程序。

## 错误处理

客户端和服务器程序都包含基本的错误处理功能，以管理网络故障、文件访问错误和无效命令等意外情况。日志和错误信息会打印到控制台，以帮助调试。

[English](README.md) | [中文](README_ZH.md)
