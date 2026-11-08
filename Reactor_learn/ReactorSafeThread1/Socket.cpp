#include "Socket.h"

int CreateSocket(int flag)
{
    int fd=-1;
    if(flag)
        fd=socket(AF_INET,SOCK_STREAM,0);
    else
        fd=socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);

    if(fd==-1) std::cout<<"CreateSocket failed"<<std::endl;
    return fd;
}

Socket::Socket(int fd):fd_(fd){}
Socket::~Socket(){ close(fd_); }

void Socket::SetBlockTime(time_t sec,time_t usec)
{
    timeval tv={sec,usec};
    setsockopt(fd_,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
}

void Socket::SetNonBlock()
{
    int state=fcntl(fd_,F_GETFL,NULL);
    if(state==-1) state=0;
    fcntl(fd_,F_SETFL,state|SOCK_NONBLOCK);
}

void Socket::SetReuseAddr(bool flag)
{
    int res=flag;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&res,sizeof(res));
}

void Socket::SetReusePort(bool flag)
{
    int res=flag;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&res,sizeof(res));
}

void Socket::SetKeepAlive(bool flag)
{
    int res=flag;
    setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&res,sizeof(res));
}

void Socket::SetTcpNoDelay(bool flag)
{
    int res=flag;
    setsockopt(fd_,SOL_SOCKET,TCP_NODELAY,&res,sizeof(res));
}

void Socket::bind(InetAddress IA){ ::bind(fd_,IA.addr(),sizeof(sockaddr)); }

void Socket::listen(int queue_len){ ::listen(fd_,queue_len); }

int Socket::accept(InetAddress& IA)
{
    sockaddr_in sai;
    socklen_t len=sizeof(sai);
    int client_fd=::accept(fd_,(sockaddr*)&sai,&len);

    IA.SetAddrIn(sai);
    if(client_fd==-1) std::runtime_error("Socket::accept failed");
    return client_fd;
}

int Socket::accept4(InetAddress& IA)
{
    sockaddr_in sai;
    socklen_t len=sizeof(sai);
    int client_fd=::accept4(fd_,(sockaddr*)&sai,&len,SOCK_NONBLOCK|SOCK_CLOEXEC);

    IA.SetAddrIn(sai);
    if(client_fd==-1) std::runtime_error("Socket::accept failed");
    return client_fd;
}

const int Socket::fd(){ return fd_; }