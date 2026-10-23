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
           
        std::function<void(int)> closecallback_;//关闭连接时的处理行为，具体的销毁行为在外部内存中进行
    private:
        void HandleClose(); //客户端关闭的处理逻辑，构造时会绑定到通道的错误事件中
        void HandleRead();  //客户端读的处理逻辑，构造时会绑定到通道的读事件中
        void HandleWrite(); //客户端写的处理逻辑，构造时会绑定到通道的写事件中
    public:
        TcpConnection(int fd,Epoll* ep);
        ~TcpConnection();
        void SetCloseCallBack(std::function<void(int)> fun);//用于从外部设置关闭连接的回调
    public:                 //与解析数据有关的业务
        void Register();    //假设的注册业务
        void Login();       //假设的登录业务
        void Other1();      //其他业务1
        void Other2();      //其他业务2...
};