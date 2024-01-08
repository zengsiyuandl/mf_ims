/*
 * Copyright (c) 2022 Siyuan Zeng
 *
 * This file is part of Mekong Fishery Inventory Management System.
 *
 * [Short Description of the File]
 *
 * Author: Siyuan Zeng
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define RETURN_OK 0
#define RETURN_ERROR 1

#define TRUE true
#define FALSE false

#define ARRLEN(array) (sizeof((array)) / sizeof((array)[0]))

#define NANO_SLEEP(x) do { \
    struct timespec ts = {0}; \
    ts.tv_sec = 0; \
    ts.tv_nsec = (x); \
    nanosleep(&ts, NULL); \
} while(0)

#define MILLI_SLEEP(x) do { \
    struct timespec ts = {0}; \
    ts.tv_sec = 0; \
    ts.tv_nsec = (x) * 1000; \
    nanosleep(&ts, NULL); \
} while(0)

#define SEC_SLEEP(x) do { \
    struct timespec ts = {0}; \
    ts.tv_sec = (x); \
    ts.tv_nsec = 0; \
    nanosleep(&ts, NULL); \
} while(0)

#define UNREFERENCE_PARAM(x) (void)(x)

int32_t strncpy_s(char *dest, size_t destsz, const char *src, size_t count);
int32_t memset_s(void *v, size_t n, int c, size_t destsz);
int32_t memcpy_s(void *dest, size_t destsz, const void *src, size_t count);
