#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

int main(){
    Socket *sockfd = new Socket();
    InetAddress *server_addr = new InetAddress("127.0.0.1", 8888);
    errif(connect(sockfd->getFd(), (sockaddr*)&server_addr->addr, server_addr->addr_len) == -1, "socket connect error");

    while(true){
        char buf[1024];
        bzero(&buf, sizeof(buf));
        scanf("%s", buf);
        ssize_t write_bytes = write(sockfd->getFd(), buf, sizeof(buf));
        if (write_bytes == -1){
            printf("disconnected");
            break;
        }
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd->getFd(), buf, sizeof(buf));
        if (read_bytes > 0){
            printf("message from server: %s\n", buf);
        }
        else if(read_bytes == 0){
            printf("server socket disconnected!\n");
            break;
        }
        else if(read_bytes == -1){
            close(sockfd->getFd());
            errif(true, "socket read error");
        }
    }
    close(sockfd->getFd());
    return 0;
}