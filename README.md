# TCP File Transmission Project

## Overview

This project implements a TCP-based file transmission system consisting of a client and a server. The client can send various commands to the server to perform operations such as listing directories, changing directories, uploading and downloading files, and resuming interrupted transmissions.

## Project Structure

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

## Prerequisites

- A C compiler (e.g., GCC)
- Make sure the following directories are present and have the necessary read/write permissions:
  - `Client/client_root`
  - `Server/server_root`

## Compilation

Use the provided `compile.sh` script to compile both the client and server programs.

```sh
chmod +x compile.sh
./compile.sh
```

This script will compile the source files and generate the executable binaries for the client and server.

## Running the Server

To start the server, use the `run_server.sh` script:

```sh
chmod +x run_server.sh
./run_server.sh
```

This will launch the server and it will start listening for client connections.

## Running the Client

To start the client, use the `run_client.sh` script:

```sh
chmod +x run_client.sh
./run_client.sh
```

The client will connect to the server and provide an interactive command-line interface for sending commands to the server.

**The script provided is compiled using clang. If you need to compile using gcc, please modify the script.**

## Commands

The client supports the following commands:

- `ls [directory]`: List the contents of the specified directory on the server.
- `cd [directory]`: Change the current directory on the server.
- `get [remote_file] [local_file]`: Download a file from the server.
- `put [local_file] [remote_file]`: Upload a file to the server.
- `rnm [old_name] [new_name]`: Rename a file on the server.
- `rmv [file_name]`: Remove a file from the server.
- `check [file_name]`: Check if a file exists on the server.
- `rget [remote_file] [local_file]`: Resume a file download from the server.
- `rput [local_file] [remote_file]`: Resume a file upload to the server.
- `help`: Display help information.
- `exit`: Exit the client program.

## Error Handling

Both the client and server programs include basic error handling to manage unexpected situations such as network failures, file access errors, and invalid commands. Logs and error messages are printed to the console to aid in debugging.

[English](README.md) | [中文](README_ZH.md)