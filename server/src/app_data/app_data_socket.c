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

int32_t AppDataStartSocket(void)
{
    return RETURN_OK;
}

void appDataUvWriteCb(uv_write_t *req, int status) {
    if (status) {
        AD_LOG_ERROR("Write error %s\n", uv_strerror(status));
    }
    free(req);
}

void appDataUvReadCb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
        uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
        AD_LOG_INFO("Received a request.");
        uv_write(req, client, &wrbuf, 1, appDataUvWriteCb);
        return;
    }
    if (nread < 0) {
        if (nread != UV_EOF)
            AD_LOG_ERROR("Read error %s\n", uv_err_name(nread));
        uv_close((uv_stream_t*)client, NULL);
    }

    free(buf->base);
}

void appDataUvAllocCb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

void appDataUvOnNewConnCb(uv_stream_t *server, int status) {
    if (status < 0) {
        AD_LOG_ERROR("New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, appDataUvAllocCb, appDataUvReadCb);
    }
    else {
        uv_close((uv_stream_t*) client, NULL);
    }
}

void *appDataStartSocketListen(void *buff)
{
    UNREFERENCE_PARAM(buff);
    uv_loop_t *loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in bind_addr;
    uv_ip4_addr(UV_DEFAULT_LISTEN_ADDR, APACHE_CONNECTION_PORT, &bind_addr);
    uv_tcp_bind(&server, (const struct sockaddr *)&bind_addr, 0);
    int32_t ret = uv_listen((uv_stream_t*)&server, UV_MAX_QUEUE_NUM, appDataUvOnNewConnCb);
    if (ret != RETURN_OK) {
        AD_LOG_ERROR("Listen error %s\n", uv_strerror(ret));
        return 1;
    }

    ret = uv_run(loop, UV_RUN_DEFAULT);
    if (ret != RETURN_ERROR) {
        AD_LOG_ERROR("uv run error. ret: %d", ret);
    }

    return NULL;
}

int32_t AppDataInitSocket(void)
{
    int32_t ret = RETURN_ERROR;

    ret = AppDataSwitchThread(NULL, appDataStartSocketListen);
    if (ret != RETURN_OK) {
        AD_LOG_ERROR("Switch thread failed. ret: %d", ret);
    }
    return ret;
}
