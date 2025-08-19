#include<stdio.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include"util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // socket返回-1报错
    errif(sockfd == -1, "socket create error");

    // 绑定服务端套接字
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");
    
    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    // epoll 事件结构体
    // typedef union epoll_data {
    //     void *ptr;
    //     int fd;          // 最常用：关联的文件描述符
    //     uint32_t u32;
    //     uint64_t u64;
    // } epoll_data_t;

    // struct epoll_event {
    //     uint32_t events;    // 监控的事件标志位
    //     epoll_data_t data;  // 用户数据
    // };
    struct epoll_event events[MAX_EVENTS], ev; // events为事件容器储存epoll_wait返回的事件信息
    bzero(&events, sizeof(events));
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd; // 事件数据关联监听套接字
    ev.events = EPOLLIN | EPOLLET; // 监控可读数据 + 边缘触发
    setnonblocking(sockfd); // sockfd 设为非阻塞模式， 让accept一次性接收完数据
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); // 将套接字添加到epoll监控

    while (true){
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); // nfds个fd发生事件
        errif(nfds == -1, "epoll wait error");
        for (int i = 0; i < nfds; ++i){
            if (events[i].data.fd == sockfd){ // 发生事件的fd是服务器socket fd，表示有新客户端连接
                // 接收客户端链接
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                bzero(&client_addr, client_addr_len);
                
                // sockaddr 为通用地址，client_addr 为特定地址类型，转换是为了 API 的通用性，可以同时支持 IPv4/IPv6/其他协议
                int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
                errif(client_sockfd == -1, "socket accept errror");
                printf("New client fd %d! IP: %s Port：%d\n", client_sockfd, 
                                                inet_ntoa(client_addr.sin_addr), 
                                                ntohs(client_addr.sin_port));

                setnonblocking(client_sockfd);

                struct epoll_event client_ev; // 局部变量声明
                bzero(&client_ev, sizeof(client_ev));
                client_ev.data.fd = client_sockfd;
                client_ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;

                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &client_ev); // 将新客户端添加到epoll
            }
            else if (events[i].events & EPOLLIN){ // 可读事件
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
    close(sockfd);
    return 0;
}