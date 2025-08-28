#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_EVENTS 10000

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

Epoll::Epoll(): epfd(-1), events(nullptr){
    epfd = epoll_create1(0);
    errif(epfd == -1, "epfd create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::Epoll(int val){
    epfd = val;
}

Epoll::~Epoll(){
    if (epfd != -1){
        close(epfd);
        epfd = -1;
    }
    delete [] events;
}


void Epoll::addFd(int fd, bool one_shot, int TRIGMode){ // fd可以是sockfd，OP为触发模式
    // 所有被epoll监听的套接字必须通过epoll_event结构体注册
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    else
        event.events = EPOLLIN | EPOLLRDHUP;

    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
} 

std::vector<epoll_event> Epoll::poll(int timeout){
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++i){
        activeEvents.push_back(events[i]); // events[i]为语法糖等价于*(events + i)
    }
    return activeEvents;
}

int Epoll::getFd(){
    return epfd;
}

epoll_event* Epoll::getEvents(){
    return events;
}