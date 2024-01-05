cmake_minimum_required(VERSION 3.0)
project(app_data)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)

# 查找 zlog 库
find_library(ZLOG_LIBRARY zlog)

# 查找 libuv 库
find_library(UV_LIBRARY uv)

if(NOT ZLOG_LIBRARY)
    message(FATAL_ERROR "zlog library not found")
endif()

if(NOT UV_LIBRARY)
    message(FATAL_ERROR "libuv library not found")
endif()

# 添加源代码文件到这个变量
set(SOURCE_FILES
    mkf_infrastructure.c
    
    app_data/app_data_log.c
    app_data/app_data.c
    app_data/app_data_mem.c
    app_data/app_data_socket.c
    app_data/app_data_thread.c
)

include_directories(
    # 假设你的所有头文件都在 src 目录下
    adapter/zlog
    app_data
)

# 创建可执行文件
add_executable(app_data ${SOURCE_FILES})

# 链接 zlog 库
target_link_libraries(app_data
    ${ZLOG_LIBRARY}
    ${UV_LIBRARY}
    pthread
)
