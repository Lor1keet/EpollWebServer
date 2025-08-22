project(common_lib)

add_library(common_lib STATIC
    Epoll.cpp
    Epoll.h
    InetAddress.cpp
    InetAddress.h
    Socket.cpp
    Socket.h
    util.cpp
    util.h
)

target_include_directories(common_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)