/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include "http_log.h" // 包含日志相关的头文件
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define SERVER_PORT 12001

static int server_socket = -1; // 全局变量用于维护与 app_data 的连接

static void close_connection() {
    if (server_socket != -1) {
        close(server_socket);
        server_socket = -1;
    }
}

static int connect_to_app_data() {
    // 这里添加建立连接到 app_data 的代码
    // 如果连接已经建立，返回现有的 socket 描述符
    // 否则，创建新的连接，并更新 server_socket
    int ret = -1;
    int sockfd;
    struct sockaddr_in servaddr;

    // 创建 socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    // 配置服务器地址
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 连接到服务器
    ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        close(sockfd);
        return -1;
    }

    server_socket = sockfd;

    return OK;
}

/* 发送数据到 app_data 并接收响应 */
static int send_to_app_data(const char *data, char *response, int response_size, request_rec *r) {
    int ret = -1;
    // 发送数据
    ret = write(server_socket, data, strlen(data));
    if (ret < 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERROR, 0, r, "mod_hello: write date failed. error: %d", errno);
    }

    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, "mod_hello: sending request to app_data success.");

    // 接收响应
    ret = read(server_socket, response, response_size);
    if (ret < 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERROR, 0, r, "mod_hello: read date failed. error: %d", errno);
    }

    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, "mod_hello: receiving app_data response success.");

    return 0;
}

static int hello_handler(request_rec *r) {
    int ret = -1;
    char response[1024] = {0};

    if (!r->handler || strcmp(r->handler, "hello_handler")) {
        return DECLINED;
    }

    if (server_socket == -1) {
        // 首次建立连接
        ret = connect_to_app_data();
        if (ret != 0 || server_socket < 0) {
            ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, "mod_hello: connect to app_data failed.");
            return -1;
        }
        ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
            "mod_hello: connect to app_data success. socket: %d", server_socket);
    }

    // 将请求数据发送到 app_data 并接收响应
    if (send_to_app_data("Hello app_data", response, sizeof(response), r) < 0) {
        close(server_socket);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ap_set_content_type(r, "text/plain");
    ap_rputs(response, r);

    return OK;
}

/* 钩子函数，将处理器函数注册到 Apache */
static void example_register_hooks(apr_pool_t *pool) {
    ap_hook_handler(hello_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* 模块定义 */
module AP_MODULE_DECLARE_DATA hello_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                  /* 创建目录配置结构 */
    NULL,                  /* 合并目录配置结构 */
    NULL,                  /* 创建服务器配置结构 */
    NULL,                  /* 合并服务器配置结构 */
    NULL,                  /* 命令表 */
    example_register_hooks /* 注册钩子 */
};

