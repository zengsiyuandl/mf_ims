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

# 路径变量
APP_DATA_EXECUTABLE="./build/app_data"
LOG_FILE="app_data.log"

# 检查 app_data 可执行文件是否存在
if [ ! -f "$APP_DATA_EXECUTABLE" ]; then
    echo "Error: $APP_DATA_EXECUTABLE does not exist."
    exit 1
fi

# 检查 app_data 是否正在运行
if pgrep -f "$APP_DATA_EXECUTABLE" > /dev/null; then
    echo "app_data is running. Stopping it..."
    pkill -f "$APP_DATA_EXECUTABLE"
fi

# 等待进程完全停止
while pgrep -f "$APP_DATA_EXECUTABLE" > /dev/null; do
    echo "Waiting for app_data to stop..."
    sleep 1
done

echo "Restarting app_data..."

# 使用 nohup 后台运行 app_data，并将输出重定向到日志文件
nohup "$APP_DATA_EXECUTABLE" > "$LOG_FILE" 2>&1 &

echo "app_data has been restarted and is running in the background."

