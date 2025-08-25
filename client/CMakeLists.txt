project(client)

add_executable(client
    client.cpp
)

target_link_libraries(client PRIVATE common_lib)

target_include_directories(client PRIVATE 
    ${CMAKE_SOURCE_DIR}/common_lib
)

target_compile_definitions(client PRIVATE 
    PROJECT_NAME="${PROJECT_NAME}"
)