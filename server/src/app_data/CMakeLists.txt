cmake_minimum_required(VERSION 3.0)
project(app_data)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)

# 查找 zlog 库
find_library(ZLOG_LIBRARY zlog)
if(NOT ZLOG_LIBRARY)
    message(FATAL_ERROR "zlog library not found")
endif()

# 创建可执行文件
add_executable(app_data app_data.c)

# 链接 zlog 库
target_link_libraries(app_data ${ZLOG_LIBRARY})

