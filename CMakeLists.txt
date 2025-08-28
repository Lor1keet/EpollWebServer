cmake_minimum_required(VERSION 3.10)
project(NetworkProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
if(WIN32 AND MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

# 包含必要的头文件路径
include_directories(
    ${CMAKE_SOURCE_DIR}/common_lib
    ${CMAKE_SOURCE_DIR}/3rd/spdlog/include
    ${CMAKE_SOURCE_DIR}/3rd/asio/asio/include
)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加common_lib库（main依赖）
add_subdirectory(common_lib)

# 仅添加main可执行目标
add_executable(main
    main.cpp
    main.h
)

# 链接common_lib库
target_link_libraries(main PRIVATE common_lib)

# 指定main需要的头文件路径
target_include_directories(main PRIVATE 
    ${CMAKE_SOURCE_DIR}/common_lib
)

# 定义编译宏（可选，根据实际需求）
target_compile_definitions(main PRIVATE 
    PROJECT_NAME="main"
)