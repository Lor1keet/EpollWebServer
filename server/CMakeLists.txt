project(server)

add_executable(server
    server.cpp
)

target_link_libraries(server PRIVATE common_lib)

target_include_directories(server PRIVATE 
    ${CMAKE_SOURCE_DIR}/common_lib
)

target_compile_definitions(server PRIVATE 
    PROJECT_NAME="${PROJECT_NAME}"
)