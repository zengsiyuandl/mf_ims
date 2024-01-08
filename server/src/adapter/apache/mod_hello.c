/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#ifdef __WIN32__
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include "apr_strings.h"
#include "apr_json.h" // 注意: 确保Apache版本支持此库
#include "http_log.h" // 包含日志相关的头文件
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define SERVER_PORT 12001
#define APACHE_MAX_SEND_BUF 1024
#define APACHE_MAX_RECEIVE_BUF 8192
#define APACHE_SOCKET_STATE_UNINITIALIZED -1

static int server_socket = APACHE_SOCKET_STATE_UNINITIALIZED; // 全局变量用于维护与 app_data 的连接

typedef enum tagAD_HTTP_METHOD_E {
    AD_HTTP_METHOD_GET = 0,
    AD_HTTP_METHOD_PUT,
    AD_HTTP_METHOD_POST,
    AD_HTTP_METHOD_DELETE,

    AD_HTTP_METHOD_BUTT
} AD_HTTP_METHOD_E;

typedef struct tagAD_SOCKET_BUF_S {
    uint8_t httpMethod;

} AD_SOCKET_BUF_S;

static void close_connection() {
    if (server_socket != APACHE_SOCKET_STATE_UNINITIALIZED) {
        close(server_socket);
        server_socket = APACHE_SOCKET_STATE_UNINITIALIZED;
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
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_hello: write date failed. error: %d", errno);
    }

    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, "mod_hello: sending request to app_data success.");

    // 接收响应
    ret = read(server_socket, response, response_size);
    if (ret < 0) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "mod_hello: read date failed. error: %d", errno);
    }

    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, "mod_hello: receiving app_data response success.");

    return 0;
}

int32_t apacheParseReqData(apr_bucket_brigade* bb, char *buf, apr_size_t *total_read)
{
    for (apr_bucket* b = APR_BRIGADE_FIRST(bb); b != APR_BRIGADE_SENTINEL(bb); b = APR_BUCKET_NEXT(b)) {
        if (APR_BUCKET_IS_EOS(b)) {
            break;
        }

        const char* data;
        apr_size_t data_len;
        apr_bucket_read(b, &data, &data_len, APR_BLOCK_READ);
        if (*total_read + data_len > APACHE_MAX_RECEIVE_BUF) {
            return HTTP_REQUEST_ENTITY_TOO_LARGE;
        }
        memcpy(buf + *total_read, data, data_len);
        *total_read += data_len;
    }
    buf[*total_read] = '\0'; // 确保字符串结束

    return OK;
}

int32_t apacheParseReqToAppData(request_rec *req)
{
    // 读取请求体
    int readBytes;
    char buf[APACHE_MAX_RECEIVE_BUF]; // 适当大小的缓冲区
    apr_size_t len = sizeof(buf);
    apr_status_t rv;
    apr_bucket_brigade* bb = apr_brigade_create(req->pool, req->connection->bucket_alloc);
    rv = ap_get_brigade(req->input_filters, bb, AP_MODE_READBYTES, APR_BLOCK_READ, len);

    if (rv != APR_SUCCESS) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 组装请求体字符串
    apr_size_t total_read = 0;
    int32_t ret = apacheParseReqData(bb, buf, &total_read);
    if (ret != OK) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, req, "Request body too large");
        return HTTP_REQUEST_ENTITY_TOO_LARGE;
    }

    // 解析JSON
    apr_json_value_t *json_req = NULL;
    int32_t json_error;
    json_req = apr_json_parse(req->pool, buf, &total_read, &json_error);
    if (!json_req) {
        // 解析错误
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, req, "Error parsing JSON request body");
        return HTTP_BAD_REQUEST;
    }

    // 提取JSON中的字段
    const char *username_key = "username";
    const char *password_key = "password";
    const char *confirm_key = "confirmedPassword";

    apr_json_value_t *username_val = apr_json_object_get(json_req, username_key, strlen(username_key));
    apr_json_value_t *password_val = apr_json_object_get(json_req, password_key, strlen(password_key));
    apr_json_value_t *confirm_val = apr_json_object_get(json_req, confirm_key, strlen(confirm_key));

    // 确保所有字段都存在且为字符串
    if (!username_val || !password_val || !confirm_val ||
        username_val->type != APR_JSON_STRING || password_val->type != APR_JSON_STRING ||
        confirm_val->type != APR_JSON_STRING) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, req, "Invalid JSON fields");
        return HTTP_BAD_REQUEST;
    }

    // 执行注册逻辑...
    // 例如，验证用户名和密码，将用户信息保存到数据库等

    // 成功处理请求
    return OK;
}

static int hello_handler(request_rec *req) {
    int ret = -1;
    char response[APACHE_MAX_SEND_BUF] = {0};

    if (!req->handler || strcmp(req->handler, "hello_handler")) {
        return DECLINED;
    }

    // 首次建立连接
    if (server_socket == APACHE_SOCKET_STATE_UNINITIALIZED) {
        ret = connect_to_app_data();
        if (ret != 0 || server_socket < 0) {
            ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, req, "mod_hello: connect to app_data failed.");
            return -1;
        }
        ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, req,
            "mod_hello: connect to app_data success. socket: %d", server_socket);
    }

    ret = apacheParseReqToAppData(req);

    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, req,
            "req: ", server_socket);

    // 将请求数据发送到 app_data 并接收响应
    if (send_to_app_data("Hello app_data", response, sizeof(response), req) < 0) {
        close(server_socket);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ap_set_content_type(req, "text/plain");
    ap_rputs(response, req);

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

