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

// zlog配置路径
// 由于不需兼容windows，该宏隔离只是为了解决在windows上使用vscode编码时的报错提示。（看着很糟心。）
#ifdef __WIN32__
#include "../thirdParty/zlog/zlog.h"
#else
#include <zlog.h>
#endif

// zlog配置
#define ZLOG_CONF_FILE_PATH "/etc/mf_imf/services/log_app_data.conf"
#define ZLOG_CATEGORY "my_cat"

#define ZLOG_MAX_LOG_LEN 1024

int32_t AppDataInitZlog(void);
int32_t AppDataZlogStartWork(void);

void AppDataLogInfo(const char *log, const char *file, const char *func, uint32_t lineNum, ...);
void AppDataLogWarning(const char *log, const char *file, const char *func, uint32_t lineNum, ...);
void AppDataLogError(const char *log, const char *file, const char *func, uint32_t lineNum, ...);
void AppDataLogDebug(const char *log, const char *file, const char *func, uint32_t lineNum, ...);
void AppDataLogFatal(const char *log, const char *file, const char *func, uint32_t lineNum, ...);

#define AD_LOG_INFO(log, ...) do {     \
    AppDataLogInfo(log, __FILE__, __func__, __LINE__, ##__VA_ARGS__);        \
} while(0);

#define AD_LOG_WARNING(log, ...) do {  \
    AppDataLogWarning(log, __FILE__, __func__, __LINE__, ##__VA_ARGS__);     \
} while(0);

#define AD_LOG_ERROR(log, ...) do {    \
    AppDataLogError(log, __FILE__, __func__, __LINE__, ##__VA_ARGS__);       \
} while(0);

#define AD_LOG_DEBUG(log, ...) do {    \
    AppDataLogDebug(log, __FILE__, __func__, __LINE__, ##__VA_ARGS__);       \
} while(0);

#define AD_LOG_FATAL(log, ...) do {    \
    AppDataLogFatal(log, __FILE__, __func__, __LINE__, ##__VA_ARGS__);       \
} while(0);
