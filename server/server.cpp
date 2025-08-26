#include <iostream>
#include "asio.hpp"
#include <string>
#include <cstring>
#include <unordered_set>

#include "Log.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

using asio::ip::tcp;

int main()
{
    asio::io_context io_context;

    tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 8888);
    tcp::acceptor acceptor(io_context, endpoint);
    
    acceptor.non_blocking(true);

    LOG_INFO("server is running");

    std::unordered_set<tcp::socket*> clients;

    while(true)
    {
        tcp::socket* new_sock = new tcp::socket(io_context);
        std::error_code ec;

        acceptor.accept(*new_sock, ec);

        if(!ec)
        {
            new_sock->non_blocking(true);
            clients.insert(new_sock);
            LOG_INFO("new client connected: {}", new_sock->remote_endpoint().address().to_string());
        }
        else
        {
            delete new_sock;
        }
            
        for(auto sock: clients)
        {
            char buf[READ_BUFFER];
            std::memset(buf, 0, sizeof(buf));

            std::error_code read_ec;
            size_t read_bytes = sock->read_some(asio::buffer(buf, sizeof(buf)), read_ec);

            if(!read_ec)
            {
                LOG_INFO("read {} bytes from client {}: {}", read_bytes, sock->remote_endpoint().address().to_string(), buf);
                size_t write_bytes = asio::write(*sock, asio::buffer(buf, read_bytes));
                LOG_INFO("write {} bytes to client {}", write_bytes, sock->remote_endpoint().address().to_string());
            }
            else if (read_ec == asio::error::eof)
            {
                LOG_INFO("client {} disconnected", sock->remote_endpoint().address().to_string());
                sock->close();
                delete sock;
                clients.erase(sock);
            }
            else if (read_ec == asio::error::would_block || read_ec == asio::error::try_again)
            {
                continue;
            }
            else
            {
                LOG_ERROR("read error from client {}: {}", sock->remote_endpoint().address().to_string(), read_ec.message());
                sock->close();
                delete sock;
                clients.erase(sock);
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    return 0;
}