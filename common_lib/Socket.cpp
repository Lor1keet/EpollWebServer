#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include"util.h"
#include"Socket.h"
#include"InetAddress.h"

Socket::Socket(): fd(-1){
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) : fd(_fd){
    errif(fd == -1, "socket create error");
}

Socket::~Socket(){

}

void Socket::bind(InetAddress *addr){
    ::bind(fd, (sockaddr*)&addr->addr, addr->addr_len);
}

void Socket::listen(){
    ::listen(fd, SOMAXCONN);
}

int Socket::accept(InetAddress* addr){
    int client_fd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
    errif(client_fd == -1, "socket accept error");
    return client_fd; 
}

int Socket::getFd(){
    return fd;
}

void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}