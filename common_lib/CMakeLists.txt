project(common_lib)

set(COMMON_SOURCES
    util.cpp
)

if(WIN32)
    set(PLATFORM_SOURCES
    )
elseif(UNIX) 
    set(PLATFORM_SOURCES
        Epoll/Epoll.cpp
        InetAddress/InetAddress.cpp
        Socket/Socket.cpp
        http/http.cpp
    )
else()
    message(FATAL_ERROR "不支持的操作系统")
endif()

set(ALL_SOURCES
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

add_library(common_lib STATIC
    ${ALL_SOURCES}
)

target_include_directories(common_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)