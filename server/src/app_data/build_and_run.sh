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

# 定义目标目录
TARGET_DIR="/etc/mf_imf/services"

# 检查 app_data 是否存在
if [ -f "$CURRENT_DIR/build/app_data" ]; then
    echo "Existing app_data found. Removing..."
    rm "$CURRENT_DIR/build/app_data"
fi

# 创建构建目录并进入
mkdir -p "$CURRENT_DIR/build"
cd "$CURRENT_DIR/build" || exit

# 运行 CMake 以配置项目并生成构建文件
cmake ..

# 编译项目
make

# 返回到脚本所在目录
cd "$CURRENT_DIR" || exit

# 检查 app_data 是否编译成功
if [ ! -f "$CURRENT_DIR/build/app_data" ]; then
    echo "Compilation failed, app_data does not exist."
    exit 1
fi

# 创建目标目录，如果它不存在
mkdir -p "$TARGET_DIR"

# 将编译好的 app_data 拷贝到目标目录
cp "$CURRENT_DIR/build/app_data" "$TARGET_DIR"

# 清除编译文件
rm -rf "$CURRENT_DIR/build"

# 检查 app_data 是否正在运行
if pgrep -f "$TARGET_DIR/app_data" > /dev/null
then
    echo "app_data is already running. Stopping it..."
    pkill -f "$TARGET_DIR/app_data"
fi

# 使用 nohup 在目标目录下后台运行 app_data，并将输出重定向到 app_data.log
cd "$TARGET_DIR"
nohup ./app_data > app_data.log 2>&1 &

echo "app_data is running in the background in $TARGET_DIR."
