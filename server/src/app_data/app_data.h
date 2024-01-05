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

#define AppDataMODULE_NAME_LEN 64

typedef struct tagAD_MODULE_MGR_S {
    char moduleName[AppDataMODULE_NAME_LEN];
    int32_t (*modInit)(void);
    int32_t (*modExit)(void);
    int32_t (*modStartWork)(void);
    int32_t (*modStopWork)(void);
    bool isInited;
    bool isStarted;
} AD_MODULE_MGR_S;
