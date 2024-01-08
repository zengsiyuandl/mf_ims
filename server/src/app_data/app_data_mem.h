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

typedef struct tagAD_PARTITION_MGR_S {
    uint32_t partId;         // 分区ID
    uint32_t size;      // // 分区大小
    uint32_t objSize;     // 每个对象的大小
    uint32_t objNum;      // 对象数量
    void* base;         // 分区基地址
    uint32_t usedNum;        // 已使用的对象数量
    bool isInitialized;  // 是否已初始化
    pthread_mutex_t lock;
} AD_PARTITION_MGR_S;

typedef enum tagAD_PARTITION_ID_E {
    AD_PARTITION_ID_PUBLIC = 0,

    AD_PARTITION_ID_BUTT
} AD_PARTITION_ID_E;

int32_t AppDataInitMemMgr(void);
int32_t AppDataMemMgrStartWork(void);

int32_t AppDataResMemPartition(uint32_t partId, uint32_t objSize, uint32_t objNum);
void *AppDataAllocMem(uint32_t partId, uint32_t objNum);
void AppDataFreeMem(uint32_t partId, uint32_t objNum, void *buffStart);

#define AD_PUB_MALLOC(ptr, memSize) do { \
    void *buff = (void *)(ptr); \
    buff = malloc((memSize)); \
} while(0)

#define AD_PUB_FREE(ptr) do { \
    free((ptr)); \
} while(0)

#define AD_ALLOC_MEM(partId) do { \
    AppDataAllocMem(partId, 1); \
} while(0)

#define AD_FREE_MEM(partId, buffStart)  do { \
    AppDataFreeMem(partId, 1, buffStart); \
} while(0)

#define AD_MULTI_ALLOC_MEM(partId, num) do { \
    AppDataAllocMem(partId, num); \
} while(0)

#define AD_MULTI_FREE_MEM(partId, buffStart, num)  do { \
    AppDataFreeMem(partId, num, buffStart); \
} while(0)
