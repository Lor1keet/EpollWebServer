#include <iostream>
#include <asio.hpp>
#include <string>
#include <cstring>

#include "Log.h"

using asio::ip::tcp;

int main()
{
    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoint = resolver.resolve("127.0.0.1","8888");

    tcp::socket socket(io_context);
    asio::connect(socket, endpoint);

    LOG_INFO("connected to server");

    while(true)
    {
        char buf[1024];
        std::memset(buf, 0, sizeof(buf));

        std::cout << "input message: ";
        std::cin >> buf;

        size_t write_bytes = asio::write(socket, asio::buffer(buf, strlen(buf)));
        LOG_INFO("write {} bytes to server", write_bytes);

        std::memset(buf, 0, sizeof(buf));

        size_t read_bytes = asio::read(socket, asio::buffer(buf, write_bytes));

        LOG_INFO("read {} bytes from server: {}", read_bytes, buf);

        if (read_bytes > 0)
        {
            LOG_INFO("server echo: {}", buf);
        }
        else if (read_bytes == 0)
        {
            LOG_WARN("server closed");
            break;
        }
        else
        {
            LOG_ERROR("read error");
            break;
        }
    }
    socket.close();
    return 0;
}