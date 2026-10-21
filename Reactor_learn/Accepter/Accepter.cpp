#include "Accepter.h"

Accepter::Accepter(std::string ip,uint16_t port,Epoll* ep):sock_(CreateSocket(0)),ch_(sock_.fd(),ep),ep_(ep)
{
    sock_.SetReuseAddr(true);
    sock_.SetReusePort(true);

    InetAddress listenIA(ip,port);
    sock_.bind(listenIA);
    sock_.listen(10);
    
    ch_.EnableReading();
    ch_.SetET();
    ch_.SetReadCallBack
    (
        [this]()
        {
            InetAddress IA;
            int fd=sock_.accept4(IA);
            if(fd!=-1&&newconnectioncallback_)
                newconnectioncallback_(fd);
        }
    );

    ep_->Update(&ch_);
}

void Accepter::SetNewConnectionCallBack(std::function<void(int)> fun){ newconnectioncallback_=fun; }

Accepter::~Accepter()
{
    ep_->Del(&ch_);
}