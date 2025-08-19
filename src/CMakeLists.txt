add_library(common_lib STATIC
    util.cpp
    util.h
)

target_include_directories(common_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(server
    server.cpp
)

target_link_libraries(server PRIVATE common_lib)

add_executable(client
    client.cpp
)

target_link_libraries(client PRIVATE common_lib)