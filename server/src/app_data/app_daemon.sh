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

# app_data 可执行文件的路径
APP_DATA_EXECUTABLE="$CURRENT_DIR/app_data"

# 日志文件路径
LOG_FILE="/var/log/app_data/app_daemon.log"

# 检查 app_data 是否正在运行的函数
is_running() {
    pgrep -f "$APP_DATA_EXECUTABLE" > /dev/null
}

# 无限循环，监控并重启 app_data
while true; do
    if ! is_running; then
        echo "$(date): app_data is not running. Restarting app_data..." >> "$LOG_FILE"
        nohup "$APP_DATA_EXECUTABLE" > /dev/null 2>&1 &
    fi
    sleep 10  # 每10秒检查一次
done

