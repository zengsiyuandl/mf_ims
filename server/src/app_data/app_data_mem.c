/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include "app_data_log.h"
#include "app_data_mem.h"

AD_PARTITION_MGR_S g_partitions[AD_PARTITION_ID_BUTT];

int32_t AppDataInitMemMgr(void)
{
    uint32_t index = 0;
    for(index; index < AD_PARTITION_ID_BUTT; index++) {
        AD_PARTITION_MGR_S *curPartition = &g_partitions[index];
        curPartition->partId = index;
        curPartition->size = 0;
        curPartition->objNum = 0;
        curPartition->objSize = 0;
        curPartition->base = NULL;
        curPartition->usedNum = 0;
        curPartition->isInitialized = FALSE;
        pthread_mutex_init(&(curPartition->lock), NULL);
    }

    return RETURN_OK;
}

int32_t AppDataMemMgrStartWork(void)
{
    return RETURN_OK;
}

int32_t AppDataResMemPartition(uint32_t partId, uint32_t objSize, uint32_t objNum)
{
    uint32_t partSize = 0;

    for (int i = 0; i < AD_PARTITION_ID_BUTT; i++) {
        // 查找一个未初始化的分区
        AD_PARTITION_MGR_S *curPartition = &g_partitions[i];
        if (curPartition->partId == partId && curPartition->isInitialized == FALSE){
            partSize =  objSize * objNum;
            curPartition->base = malloc(partSize); // 分配内存
            if (curPartition->base == NULL) {
                AD_LOG_ERROR("Reserve memory for partition %u failed.", partId);
                return RETURN_ERROR;
            }
            (void)memset_s(curPartition->base, partSize, 0, partSize);
            curPartition->objSize = objSize;
            curPartition->objNum = objNum;
            curPartition->size = objSize * objNum; // 计算总大小
            curPartition->usedNum = 0;
            curPartition->isInitialized = TRUE; // 标记为已初始化
            break; // 初始化完毕后退出循环
        }
    }

    return RETURN_OK;
}

void *AppDataAllocMem(uint32_t partId, uint32_t objNum)
{
    void *allocated = NULL;

    for (int i = 0; i < AD_PARTITION_ID_BUTT; i++) {
        if (g_partitions[i].partId == partId) {
            pthread_mutex_lock(&g_partitions[i].lock); // 加锁特定分区

            AD_PARTITION_MGR_S *curPartition = &g_partitions[i];
            if (curPartition->isInitialized == TRUE) {
                if (curPartition->usedNum + objNum <= curPartition->objNum) {
                    void *objStart = (char *)curPartition->base + curPartition->usedNum * curPartition->objSize;
                    curPartition->usedNum += objNum;
                    allocated = objStart;
                } else {
                    AD_LOG_ERROR("Buff allocated is out of range in partId(%u). buff used num:%u, buff needed: %u",
                        partId, curPartition->usedNum, objNum);
                    pthread_mutex_unlock(&g_partitions[i].lock); // 解锁特定分区
                    return NULL;
                }
            } else {
                AD_LOG_ERROR("PartId %u has not init yet", partId);
                pthread_mutex_unlock(&g_partitions[i].lock); // 解锁特定分区
                return NULL;
            }

            pthread_mutex_unlock(&g_partitions[i].lock); // 解锁特定分区
            break;
        }
    }
    return allocated;
}

void AppDataFreeMem(uint32_t partId, uint32_t objNum, void *buffStart)
{
    for (int i = 0; i < AD_PARTITION_ID_BUTT; i++) {
        if (g_partitions[i].partId == partId) {
            pthread_mutex_lock(&g_partitions[i].lock); // 加锁特定分区

            AD_PARTITION_MGR_S *curPartition = &g_partitions[i];
            if ((char *)buffStart >= (char *)curPartition->base &&
                (char *)buffStart < (char *)curPartition->base + curPartition->size) {
                char *memStart = (char *)buffStart;
                size_t memSize = curPartition->objSize * objNum;
                
                // 将内存清零
                (void)memset_s(memStart, memSize, 0, memSize);
                curPartition->usedNum -= objNum;
            }

            pthread_mutex_unlock(&g_partitions[i].lock); // 解锁特定分区
            break;
        }
    }
}

void AppDataDestroyMemMgr(void)
{
    for (uint32_t index = 0; index < AD_PARTITION_ID_BUTT; index++) {
        pthread_mutex_destroy(&g_partitions[index].lock);
    }
}
