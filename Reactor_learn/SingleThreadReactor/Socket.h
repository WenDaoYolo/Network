#pragma once
#include<netinet/tcp.h>
#include<sys/socket.h>
#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include "InetAddress.h"


int CreateSocket(int flag);//1.block 0.nonblock

class Socket
{
    private:
        int fd_;
    public:
        Socket(int fd);
        ~Socket();

        void SetNonBlock();
        void SetReuseAddr(bool flag);
        void SetReusePort(bool flag);
        void SetKeepAlive(bool flag);
        void SetTcpNoDelay(bool flag);
        void SetBlockTime(time_t sec,time_t usec);

        const int fd();
        void bind(InetAddress IA);
        void listen(int queue_len);
        int accept(InetAddress& IA);
        int accept4(InetAddress& IA);
};