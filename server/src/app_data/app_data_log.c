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

zlog_category_t *zlogCategory;

int32_t AppDataInitZlog(void)
{
    zlogCategory = NULL;
    // 初始化 zlog
    if (zlog_init(ZLOG_CONF_FILE_PATH)) {
        fprintf(stderr, "zlog init failed\n");
        return RETURN_ERROR;
    }

    zlog_category_t *c = zlog_get_category(ZLOG_CATEGORY);
    if (!c) {
        fprintf(stderr, "zlog get category failed\n");
        zlog_fini();
        return RETURN_ERROR;
    }
    
    zlogCategory = c;
    if (zlogCategory == NULL) {
        return RETURN_ERROR;
    }
    return RETURN_OK;
}

int32_t AppDataZlogStartWork(void)
{
    return RETURN_OK;
}

// LOGINFO
void AppDataLogInfo(const char *log, const char *file, const char *func, uint32_t lineNum, ...)
{
    if (zlogCategory == NULL || strlen(log) >= ZLOG_MAX_LOG_LEN) {
        return;
    }

    va_list args;
    va_start(args, lineNum);

    char formatted_log[ZLOG_MAX_LOG_LEN] = {0};
    char buffer[ZLOG_MAX_LOG_LEN] = {0};

    // 使用vsnprintf将log和可变参数格式化到buffer中
    vsnprintf(buffer, ZLOG_MAX_LOG_LEN, log, args);
    size_t needed = snprintf(NULL, 0, "[%s][%s, %s:%u]", buffer, file, func, lineNum);
    if (needed >= ZLOG_MAX_LOG_LEN) {
        // 处理可能的截断
    }

    // 将文件名、行号和格式化后的消息合并到formatted_log中
    snprintf(formatted_log, ZLOG_MAX_LOG_LEN, "[%s][%s, %s:%u]", buffer, file, func, lineNum);

    zlog_info(zlogCategory, "%s", formatted_log);

    va_end(args);
}

// LogWarning
void AppDataLogWarning(const char *log, const char *file, const char *func, uint32_t lineNum, ...)
{
    if (zlogCategory == NULL || strlen(log) >= ZLOG_MAX_LOG_LEN) {
        return;
    }

    va_list args;
    va_start(args, lineNum);

    char formatted_log[ZLOG_MAX_LOG_LEN] = {0};
    char buffer[ZLOG_MAX_LOG_LEN] = {0};

    // 使用vsnprintf将log和可变参数格式化到buffer中
    vsnprintf(buffer, ZLOG_MAX_LOG_LEN, log, args);
    size_t needed = snprintf(NULL, 0, "[%s][%s, %s:%u]", buffer, file, func, lineNum);
    if (needed >= ZLOG_MAX_LOG_LEN) {
        // 处理可能的截断
    }

    // 将文件名、行号和格式化后的消息合并到formatted_log中
    snprintf(formatted_log, ZLOG_MAX_LOG_LEN, "[%s][%s, %s:%u]", buffer, file, func, lineNum);

    zlog_warn(zlogCategory, "%s", formatted_log);

    va_end(args);
}

// LogError
void AppDataLogError(const char *log, const char *file, const char *func, uint32_t lineNum, ...)
{
    if (zlogCategory == NULL || strlen(log) >= ZLOG_MAX_LOG_LEN) {
        return;
    }

    va_list args;
    va_start(args, lineNum);

    char formatted_log[ZLOG_MAX_LOG_LEN] = {0};
    char buffer[ZLOG_MAX_LOG_LEN] = {0};

    // 使用vsnprintf将log和可变参数格式化到buffer中
    vsnprintf(buffer, ZLOG_MAX_LOG_LEN, log, args);
    size_t needed = snprintf(NULL, 0, "[%s][%s, %s:%u]", buffer, file, func, lineNum);
    if (needed >= ZLOG_MAX_LOG_LEN) {
        // 处理可能的截断
    }

    // 将文件名、行号和格式化后的消息合并到formatted_log中
    snprintf(formatted_log, ZLOG_MAX_LOG_LEN, "[%s][%s, %s:%u]", buffer, file, func, lineNum);

    zlog_error(zlogCategory, "%s", formatted_log);

    va_end(args);
}

//LogDebug
void AppDataLogDebug(const char *log, const char *file, const char *func, uint32_t lineNum, ...)
{
    if (zlogCategory == NULL || strlen(log) >= ZLOG_MAX_LOG_LEN) {
        return;
    }

    va_list args;
    va_start(args, lineNum);

    char formatted_log[ZLOG_MAX_LOG_LEN] = {0};
    char buffer[ZLOG_MAX_LOG_LEN] = {0};

    // 使用vsnprintf将log和可变参数格式化到buffer中
    vsnprintf(buffer, ZLOG_MAX_LOG_LEN, log, args);
    size_t needed = snprintf(NULL, 0, "[%s][%s, %s:%u]", buffer, file, func, lineNum);
    if (needed >= ZLOG_MAX_LOG_LEN) {
        // 处理可能的截断
    }

    // 将文件名、行号和格式化后的消息合并到formatted_log中
    snprintf(formatted_log, ZLOG_MAX_LOG_LEN, "[%s][%s, %s:%u]", buffer, file, func, lineNum);

    zlog_debug(zlogCategory, "%s", formatted_log);

    va_end(args);
}

// LogFatal
void AppDataLogFatal(const char *log, const char *file, const char *func, uint32_t lineNum, ...)
{
    if (zlogCategory == NULL) {
        return;
    }

    va_list args;
    va_start(args, lineNum);

    char formatted_log[ZLOG_MAX_LOG_LEN] = {0};
    char buffer[ZLOG_MAX_LOG_LEN] = {0};

    // 使用vsnprintf将log和可变参数格式化到buffer中
    vsnprintf(buffer, ZLOG_MAX_LOG_LEN, log, args);
    size_t needed = snprintf(NULL, 0, "[%s][%s, %s:%u]", buffer, file, func, lineNum);
    if (needed >= ZLOG_MAX_LOG_LEN) {
        // 处理可能的截断
    }

    // 将文件名、行号和格式化后的消息合并到formatted_log中
    snprintf(formatted_log, ZLOG_MAX_LOG_LEN, "[%s][%s, %s:%u]", buffer, file, func, lineNum);

    zlog_fatal(zlogCategory, "%s", formatted_log);

    va_end(args);
}
