/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include "app_data.h"
#include "app_data_log.h"
#include "app_data_socket.h"
#include "app_data_mem.h"

int32_t appDataEndInit(void)
{
    return RETURN_OK;
}

int32_t appDateEndStartWork(void)
{
    return RETURN_OK;
}

AD_MODULE_MGR_S g_appDataModules[] = {
    {"zlog",       AppDataInitZlog,     NULL,     AppDataZlogStartWork,     NULL, FALSE, FALSE},
    {"mem",        AppDataInitMemMgr,   NULL,     AppDataMemMgrStartWork,   NULL, FALSE, FALSE},
    {"socket",     AppDataInitSocket,   NULL,     AppDataStartSocket,       NULL, FALSE, FALSE},

    {"endInit",    appDataEndInit,      NULL,     appDateEndStartWork,      NULL, FALSE, FALSE} // 使用 end 标记数组结束
};

int32_t appDateInit(void)
{
    uint32_t index = 0;
    uint32_t modNum = ARRLEN(g_appDataModules);
    AD_MODULE_MGR_S *modIter = NULL;
    int32_t ret = RETURN_ERROR;

    for (index = 0; index < modNum; index++) {
        modIter = &(g_appDataModules[index]);
        if (modIter->isInited == TRUE) {
            continue;
        }
        if (modIter->modInit == NULL) {
            AD_LOG_WARNING("module num: %d doesn't have a init function.", index);
            continue;
        } else {
            AD_LOG_INFO("module: %s starts initing.", modIter->moduleName);
            ret = modIter->modInit();
            if (ret != RETURN_OK) {
                if (modIter->modExit != NULL) {
                    modIter->modExit();
                    modIter->isInited = FALSE;
                }
                AD_LOG_ERROR("module: %s inited failed.", modIter->moduleName);
                return RETURN_ERROR;
            }
            modIter->isInited = TRUE;
            AD_LOG_INFO("module: %s inited success.", modIter->moduleName);
        }
    }
    
    return RETURN_OK;
}

int32_t appDateStartWork(void)
{
    uint32_t index = 0;
    uint32_t modNum = ARRLEN(g_appDataModules);
    AD_MODULE_MGR_S *modIter = NULL;
    int32_t ret = RETURN_ERROR;

    for (index = 0; index < modNum; index++) {
        modIter = &(g_appDataModules[index]);
        if (modIter->isStarted == TRUE) {
            continue;
        }
        if (modIter->modStartWork == NULL || modIter->moduleName == NULL) {
            AD_LOG_WARNING("module num: %d doesn't have a startwork function.", index);
            continue;
        } else {
            AD_LOG_INFO("module: %s is startworking.", modIter->moduleName);
            ret = modIter->modStartWork();
            if (ret != RETURN_OK) {
                if (modIter->modStopWork != NULL) {
                    modIter->modStopWork();
                    modIter->isStarted = FALSE;
                }
                AD_LOG_ERROR("module: %s startwork failed.", modIter->moduleName);
                return RETURN_ERROR;
            }
            modIter->isStarted = TRUE;
            AD_LOG_INFO("module: %s startwork success.", modIter->moduleName);
        }
    }

    return RETURN_OK;
}

int32_t main()
{

    int32_t ret = RETURN_ERROR;

    do {
        ret = appDateInit();
        if (ret != RETURN_OK) {
            AD_LOG_FATAL("APP DATA INIT FAILED. EXIT.");
            break;
        }

        ret = appDateStartWork();
        if (ret != RETURN_OK) {
            AD_LOG_FATAL("APP DATA STARTWORK FAILED. EXIT.");
            break;
        }
        SEC_SLEEP(3);
    } while (TRUE);

    return RETURN_ERROR;
}
