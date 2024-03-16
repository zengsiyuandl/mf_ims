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
#include <uv.h>

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
#define UV_MAX_QUEUE_NUM 128
#define UV_DEFAULT_LISTEN_ADDR "127.0.0.1"

#define AD_MSG_OBJ_JSON_MAX_KEY_BUFF 64
#define AD_MSG_OBJ_JSON_MAX_VALUE_BUFF 256
#define AD_MSG_OBJ_JSON_MAX_NUM 16

typedef enum tag_AD_MSG_OBJ_OPCODE_E {
    AD_MSG_OBJ_METHOD_GET,
    AD_MSG_OBJ_METHOD_CREATE,
    AD_MSG_OBJ_METHOD_UPDATE,
    AD_MSG_OBJ_METHOD_DELETE,

    AD_MSG_OBJ_METHOD_BUTT
} AD_MSG_OBJ_OPCODE_E;

typedef struct tag_AD_MSG_OBJ_JSON_S {
    char jsonKeyBuf[AD_MSG_OBJ_JSON_MAX_KEY_BUFF];
    char jsonValueBuf[AD_MSG_OBJ_JSON_MAX_VALUE_BUFF];
} AD_MSG_OBJ_JSON_S;

typedef struct tagAD_MSG_OBJ_S {
    uint32_t objId;
    uint32_t httpMethod;
    AD_MSG_OBJ_JSON_S jsonObj[AD_MSG_OBJ_JSON_MAX_NUM];
} AD_MSG_OBJ_S;

int32_t AppDataInitSocket(void);
int32_t AppDataStartSocket(void);
