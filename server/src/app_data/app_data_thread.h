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
#include "app_data_log.h"

int32_t AppDataSwitchThread(void *buff, void *(*threadFunc)(void *buff));
