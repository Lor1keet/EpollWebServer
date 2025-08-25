project(common_lib)
set(CMAKE_CXX_STANDARD 20)
file(GLOB COMMON_SOURCES "*.cpp")
file(GLOB COMMON_HEADERS "*.h")

add_library(common_lib STATIC
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

target_include_directories(common_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)