#ifndef _EPOLL_H_
#define _EPOLL_H_

#include<sys/epoll.h>
#include<vector>

class Epoll{
public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);
    std::vector<epoll_event> poll(int timeout = -1);

private:
    int epfd;
    struct epoll_event *events;
};

#endif _EPOLL_H_