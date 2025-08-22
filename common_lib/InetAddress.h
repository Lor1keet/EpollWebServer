#ifndef _INETADDRESS_H_
#define _INETADDRESS_H_
#include<arpa/inet.h>

class InetAddress{
public:
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    InetAddress();
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress();

};

#endif _INETADDRESS_H_