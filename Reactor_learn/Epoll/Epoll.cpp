#include "Epoll.h"

Epoll::Epoll()
{
    epfd_=epoll_create(1);
    if(epfd_==-1) std::cout<<"epoll_create faild"<<std::endl;
}

Epoll::~Epoll(){ close(epfd_); }

void Epoll::AddFd(int fd,int op)
{
    epoll_event ev;
    ev.data.fd=fd;
    ev.events=op;

    if(epoll_ctl(epfd_,EPOLL_CTL_ADD,fd,&ev)==-1)
        std::cout<<"epoll_ctl ADD failed"<<std::endl;
}

std::vector<epoll_event> Epoll::loop(int timeout)
{
    int events=epoll_wait(epfd_,revents_,maxEvents_,timeout);
    std::vector<epoll_event> res;

    if(events<0)
    {
        if(errno!=EINTR)
            std::cout<<"call epoll_wait failed"<<std::endl;
    }
    else if(events==0)
    {
        std::cout<<"no event"<<std::endl;
    }
    else
    {
        for(int i=0;i<events;i++)
            res.push_back(revents_[i]);
    }
    return res;
}

void Epoll::DelFd(int fd)
{
    if(epoll_ctl(epfd_,EPOLL_CTL_DEL,fd,NULL)==-1)
        std::cout<<"epoll_ctl EDL failed"<<std::endl;
}