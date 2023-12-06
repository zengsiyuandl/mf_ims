/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <zlog.h>

#define PORT 12001

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello, World!";

    // 初始化 zlog
    if (zlog_init("/etc/mf_imf/services/log_app_data.conf")) {
        fprintf(stderr, "zlog init failed\n");
        return -1;
    }

    zlog_category_t *c = zlog_get_category("my_cat");
    if (!c) {
        fprintf(stderr, "zlog get category failed\n");
        zlog_fini();
        return -2;
    }

    zlog_info(c, "app_data starting...");

    // 创建 socket 文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        zlog_fatal(c, "socket failed");
        exit(EXIT_FAILURE);
    }

    // 绑定 socket 到端口 12001
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        zlog_fatal(c, "setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        zlog_fatal(c, "bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        zlog_fatal(c, "listen");
        exit(EXIT_FAILURE);
    }

    zlog_info(c, "Listening on port %d...", PORT);

    while(1) {
        // 接受新的连接
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            zlog_fatal(c, "accept");
            continue;  // 继续监听其他连接
        }
        read(new_socket, buffer, 1024);
        zlog_info(c, "Received message: %s", buffer);
        send(new_socket, hello, strlen(hello), 0);
        zlog_info(c, "Hello message sent");
        close(new_socket);
    }

    // 关闭 socket，清理 zlog（理论上不会执行到这里）
    close(server_fd);
    zlog_fini();

    return 0;
}
