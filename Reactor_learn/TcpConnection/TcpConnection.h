#pragma once
#include "Epoll.h"
#include "Socket.h"
#include "Channel.h"
#include "RecvBuffer.h"
#include "InetAddress.h"
#include<functional> 

class TcpConnection
{
    private:
        Epoll* ep_;
        Socket sock_;
        Channel ch_;
        RecvBuffer buffer_;
           
        std::function<void(int)> closecallback_;//destroy in extern memory
    private:
        void HandleClose();
        void HandleRead();
        void HandleWrite();
    public:
        TcpConnection(int fd,Epoll* ep);
        ~TcpConnection();
        void SetCloseCallBack(std::function<void(int)> fun);
};