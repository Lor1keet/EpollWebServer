#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

#include "Log.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(){
    Socket *server_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    server_sock->bind(serv_addr);
    server_sock->listen();

    Epoll *ep = new Epoll();
    server_sock->setnonblocking();
    ep->addFd(server_sock->getFd(), EPOLLIN | EPOLLET); // 将server_sock的fd添加到epoll

    LOG_INFO("server is running");

    while(true){
        std::vector<epoll_event> events = ep->poll(); // epoll等待事件
        int nfds = events.size();
        for (int i = 0; i < nfds; ++i){
            if (events[i].data.fd == server_sock->getFd()){ // 若发生事件的fd是服务器socket fd，表示有新客户端连接
                InetAddress *client_addr = new InetAddress();
                Socket *client_sock = new Socket(server_sock->accept(client_addr));
                printf("new client fd %d! IP: %s Port: %d\n", client_sock->getFd(), inet_ntoa(client_addr->addr.sin_addr), ntohs(client_addr->addr.sin_port));
                client_sock->setnonblocking();
                ep->addFd(client_sock->getFd(), EPOLLIN | EPOLLET);
            }
            else if (events[i].data.fd && EPOLLIN){ // 若有事件写入则处理
                char buf[READ_BUFFER];
                while(true){
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf)); 
                    if(bytes_read > 0){
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, bytes_read);
                    } 
                    else if(bytes_read == -1 && errno == EINTR){  // 客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } 
                    else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){ // 非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } 
                    else if(bytes_read == 0){  // EOF，客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);   // 关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            }
            
        }
    }
    delete server_sock;
    delete serv_addr;
    return 0;
}