#pragma once
#include "Channel.h"
#include "Socket.h"
#include "Epoll.h"
#include "InetAddress.h"

class Accepter
{
    private:
        Socket sock_;
        Channel ch_;
        Epoll* ep_;
        std::function<void(int,InetAddress)> newconnectioncallback_;
    public:
        Accepter(std::string ip,uint16_t port,Epoll* ep);
        void SetNewConnectionCallBack(std::function<void(int,InetAddress)> fun);
        ~Accepter();
};