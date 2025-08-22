#ifndef _SOCKET_H_
#define _SOCKET_H_

class InetAddress;
class Socket{
public:
    Socket(); // 默认
    Socket(int); // 接收
    ~Socket();

    void bind(InetAddress*);
    void listen();
    void setnonblocking();
    int accept(InetAddress*);

    int getFd();

private:
    int fd;
};

#endif _SOCKET_H_
