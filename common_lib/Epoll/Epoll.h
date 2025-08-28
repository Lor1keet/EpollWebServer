#ifndef _EPOLL_H_
#define _EPOLL_H_

#include<sys/epoll.h>
#include<vector>

class Epoll{
public:
    Epoll();
    Epoll(int);
    ~Epoll();

    void addFd(int fd, bool one_shot, int TRIGMode);
    std::vector<epoll_event> poll(int timeout = -1);
    int getFd();
    epoll_event* getEvents();
private:
    int epfd;
    struct epoll_event *events;
};

#endif _EPOLL_H_