#include "TcpConnection.h"

TcpConnection::TcpConnection(int fd,Epoll* ep):ep_(ep),sock_(fd),ch_(fd,ep)
{    
    ch_.EnableReading();
    ch_.SetET();
    
    ch_.SetErrorCallBack( [this](){ HandleClose(); } );
    ch_.SetReadCallBack( [this](){ HandleRead(); } );
    ch_.SetWriteCallBack( [this](){ HandleWrite(); } );

    ep->Update(&ch_);
}

void TcpConnection::SetCloseCallBack(std::function<void(int)> fun){ closecallback_=fun; }

TcpConnection::~TcpConnection(){}


void TcpConnection::HandleClose()
{
    if(ch_.inepoll())
        ep_->Del(&ch_);
    else 
        return;
    
    if(closecallback_) closecallback_(sock_.fd());
}

void TcpConnection::HandleRead()
{
    while(true)
    {
        size_t readables=buffer_.CheckWritAble(1024);
        int nums=recv(sock_.fd(),buffer_.WritePtr(),readables,0);

        if(nums<0)
        {
            if(errno==EAGAIN) break;
            std::cout<<"call recv failed"<<std::endl;
            HandleClose();
            return;
        }
        else if(nums==0)
        {
            HandleClose();
            return;
        }
        else
        {
            buffer_.MoveWritePtr(nums);
        }
    }

    while(buffer_.ReadAbleBytes()>=4)
    {
        int len=0;
        memcpy(&len,buffer_.ReadPtr(),sizeof(int));
        len=ntohl(len);
        
        if(buffer_.ReadAbleBytes()>=4+len)
        {
            buffer_.MoveReadPrt(4);
            std::string msg(buffer_.ReadPtr(),len);
            buffer_.MoveReadPrt(len);
            std::cout<<"client"<<sock_.fd()<<":"<<msg<<std::endl;
        }
        else
            break;
    }
}

void TcpConnection::HandleWrite()
{
    //...
}