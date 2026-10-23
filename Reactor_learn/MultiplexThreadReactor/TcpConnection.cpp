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
        return;//如果不在红黑树则直接退出，防止重复调用关闭连接的回调
    
    if(closecallback_) closecallback_(sock_.fd());//关闭后该内存会销毁，不要再执行仍和访问操作
}

//////////////////////////////////////////////////
void TcpConnection::Register()
{
    //to do...
    char buf[]="register success";
    int nums=send(sock_.fd(),buf,sizeof(buf),0);
}

void TcpConnection::Login()
{
    //to do...
    char buf[]="login success";
    send(sock_.fd(),buf,sizeof(buf),0);
}

void TcpConnection::Other1()
{
    //to do...
    char buf[]="other1 success";
    send(sock_.fd(),buf,sizeof(buf),0);
}

void TcpConnection::Other2()
{
    //to do...
    char buf[]="other2 success";
    send(sock_.fd(),buf,sizeof(buf),0);
}
//////////////////////////////////////////////////

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
            return;//同理内存会销毁，直接返回，不要再执行任何访问操作
        }
        else if(nums==0)
        {
            HandleClose();
            return;//同理内存会销毁，直接返回，不要再执行任何访问操作
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
            
            if(!strcmp(msg.c_str(),"register"))
                Register();
            if(!strcmp(msg.c_str(),"login"))
                Login();
            if(!strcmp(msg.c_str(),"other1"))
                Other1();
            if(!strcmp(msg.c_str(),"other2"))
                Other2();
        }
        else
            break;//如果数据不完整则跳出循环，等待下次解析
    }
}

void TcpConnection::HandleWrite()
{
    //...
}