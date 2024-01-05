/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include "app_data_socket.h"
#include "app_data_mem.h"
#include "app_data_log.h"
#include "app_data_thread.h"

#define MAX_BUF_TO_APACHE 1024
#define MAX_SOCK_ADDR_NUM 1

int32_t g_serverFd;

struct sockaddr_in *g_sockAddress;

void *appDataRecvApaReq(void* buff)
{
    UNREFERENCED_PARAM(buff);

    int32_t new_socket = 0;
    int32_t addrlen = sizeof(struct sockaddr_in);
    char buffer[MAX_BUF_TO_APACHE] = {0};
    AD_LOG_INFO("Socket startwork success.");
    while (1) {
        // 接受新的连接
        if ((new_socket = accept(g_serverFd, (struct sockaddr *)g_sockAddress, (socklen_t*)&addrlen)) < 0) {
            AD_LOG_ERROR("accept failed.");
            continue;  // 继续监听其他连接
        }
        read(new_socket, buffer, MAX_BUF_TO_APACHE);
        AD_LOG_INFO("Received message: %s", buffer);
        send(new_socket, "hello", strlen("hello"), 0);
        AD_LOG_INFO("Hello message sent");
        close(new_socket);
    }

    return NULL;
}

int32_t AppDataStartSocket(void)
{
    int32_t ret = RETURN_OK;

    AD_LOG_INFO("Socket is startworking.");
    ret = AppDataSwitchThread(NULL, appDataRecvApaReq);
    if (ret != RETURN_OK) {
        AD_LOG_ERROR("Start listen to apache failed. ret: %d.", ret);
    }
    return ret;
}

int32_t AppDataInitSocket(void)
{
    struct sockaddr_in address;
    uint32_t addrSize = sizeof(struct sockaddr_in);
    int32_t server_fd = 0;
    int32_t opt = 1;
    g_serverFd = 0;

    AD_LOG_INFO("Socket is initing.");
    int32_t ret = AppDataResMemPartition(AD_PARTITION_ID_SOCKE_ADDR, addrSize, MAX_SOCK_ADDR_NUM);
    if (ret != RETURN_OK) {
        AD_LOG_ERROR("socket mem init failed.");
        return RETURN_ERROR;
    }
    
    (void)memset_s(g_sockAddress, sizeof(struct sockaddr_in), 0, sizeof( struct sockaddr_in));

    // 创建 socket 文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        AD_LOG_ERROR("socket failed");
        return RETURN_ERROR;
    }

    // 绑定 socket 到端口 12001
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | AppDataSO_REUSEPORT, &opt, sizeof(opt))) {
        AD_LOG_ERROR("setsockopt");
        return RETURN_ERROR;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(APACHE_CONNECTION_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        AD_LOG_ERROR("bind failed.");
        return RETURN_ERROR;
    }

    if (listen(server_fd, 3) < 0) {
        AD_LOG_ERROR("listen failed.");
        return RETURN_ERROR;
    }

    (void)memcpy_s(g_sockAddress, sizeof(struct sockaddr_in), &address, sizeof(struct sockaddr_in));
    g_serverFd = server_fd;
    AD_LOG_INFO("Socket init success.");
    return RETURN_OK;
}
