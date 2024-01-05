/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include "mkf_infrastructure.h"

int32_t strncpy_s(char *dest, size_t destsz, const char *src, size_t count)
{
    // 参数检查
    if(dest == NULL || src == NULL) return EINVAL;
    if(destsz == 0 || count == 0) return ERANGE;

    // 确保目标缓冲区足够大
    size_t i;
    for(i = 0; i < count && i < destsz - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    // 如果源字符串长度小于count，填充剩余的dest字符为'\0'
    for (; i < destsz; i++) {
        dest[i] = '\0';
    }

    return RETURN_OK;
}

int32_t memset_s(void *v, size_t n, int c, size_t destsz)
{
    if (v == NULL) return EINVAL;
    if (n > destsz) return ERANGE;

    unsigned char *p = v;
    while (destsz-- && n--) {
        *p++ = (unsigned char)c;
    }

    return RETURN_OK;
}

int32_t memcpy_s(void *dest, size_t destsz, const void *src, size_t count)
{
    if (dest == NULL || src == NULL) return EINVAL;
    if (count > destsz) return ERANGE;

    char *cdest = (char *)dest;
    const char *csrc = (const char *)src;

    while (count--) {
        *cdest++ = *csrc++;
    }

    return RETURN_OK;
}
