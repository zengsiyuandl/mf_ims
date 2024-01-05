/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include "../mkf_infrastructure.h"

// socket编程相关头文件，由于不需兼容windows，该宏隔离只是为了解决在windows上使用vscode编码时的报错提示。（看着很糟心。）
#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef __WIN32__
#define AppDataSO_REUSEPORT 0
#else
#define AppDataSO_REUSEPORT SO_REUSEPORT
#endif


#ifdef __WIN32
typedef int socklen_t;
#endif

// apache连接app_data使用的端口
#define APACHE_CONNECTION_PORT 12001

int32_t AppDataInitSocket(void);
int32_t AppDataStartSocket(void);
