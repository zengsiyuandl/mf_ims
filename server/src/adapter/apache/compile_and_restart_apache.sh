#!/bin/bash
# ----------------------------------------------------------------------------------
# Copyright (c) 2022 Siyuan Zeng
#
# This script is part of Mekong Fishery Inventory Management System.
#
# [Short Description of the Script]
#
# Author: Siyuan Zeng
# ----------------------------------------------------------------------------------

# 获取当前脚本所在的目录
CURRENT_DIR=$(dirname "$0")

# 定义模块文件名和模块名称
MOD_FILE="$CURRENT_DIR/mod_hello.c"
MOD_NAME="mod_hello.so"

# 检查 mod_hello.c 文件是否存在
if [ ! -f "$MOD_FILE" ]; then
    echo "Error: $MOD_FILE does not exist."
    exit 1
fi

# 编译 mod_hello.c
echo "Compiling $MOD_FILE..."
apxs -i -a -c "$MOD_FILE"

# 检查模块是否编译成功
if [ ! -f "/usr/lib/apache2/modules/$MOD_NAME" ]; then
    echo "Error: Compilation of $MOD_NAME failed."
    exit 1
fi

a2enconf mod_hello

# 重启 Apache 服务
echo "Restarting Apache..."
service apache2 restart

# 检查 Apache 服务是否重启成功
if service apache2 status | grep -q 'running'; then
    echo "Apache successfully restarted with $MOD_NAME."
else
    echo "Error: Apache failed to restart."
    exit 1
fi
