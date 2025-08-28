cmake_minimum_required(VERSION 3.10)
project(NetworkProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
if(WIN32 AND MSVC)
    target_compile_options(your_target PRIVATE /utf-8)
endif()
include_directories(
    ${CMAKE_SOURCE_DIR}/common_lib
    ${CMAKE_SOURCE_DIR}/3rd/spdlog/include
    ${CMAKE_SOURCE_DIR}/3rd/asio/asio/include
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(common_lib)
add_subdirectory(client)
add_subdirectory(server)
