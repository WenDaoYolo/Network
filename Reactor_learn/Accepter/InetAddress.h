#pragma once
#include<iostream>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>

class InetAddress
{
    private:
        sockaddr_in sai;
        
    public:
        InetAddress(const std::string ip,uint16_t port);
        InetAddress();
        InetAddress(const sockaddr_in& other);
        void SetAddrIn(sockaddr_in other);                                 
        ~InetAddress();

        const sockaddr* addr();
        const std::string ip();
        uint16_t port();
};