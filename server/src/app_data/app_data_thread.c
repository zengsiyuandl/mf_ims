/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include "app_data_thread.h"

int32_t AppDataSwitchThread(void *buff, void *(*threadFunc)(void *buff))
{
    pthread_t thread_id;
    int32_t ret = RETURN_ERROR;

    // 创建线程
    ret = pthread_create(&thread_id, NULL, threadFunc, buff);
    if (ret != 0) {
        AD_LOG_ERROR("Switch to new thread failed. ret: %d.", ret);
        return RETURN_ERROR;
    }

    return RETURN_OK;
}
