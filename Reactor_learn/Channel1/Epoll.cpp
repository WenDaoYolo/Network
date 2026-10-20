#include "Epoll.h"
#include "Channel.h"

Epoll::Epoll()
{
    epfd_=epoll_create(1);
    if(epfd_==-1) std::cout<<"epoll_create faild"<<std::endl;
}

Epoll::~Epoll(){ close(epfd_); }

void Epoll::Update(Channel* ch)
{
    epoll_event ev;
    ev.data.ptr=ch;
    ev.events=ch->events();

    if(ch->inepoll())
    {
        if(epoll_ctl(epfd_,EPOLL_CTL_MOD,ch->fd(),&ev)==-1)
            std::cout<<"epoll_ctl ADD failed"<<std::endl;
    }
    else
    {
        if(epoll_ctl(epfd_,EPOLL_CTL_ADD,ch->fd(),&ev)==-1)
            std::cout<<"epoll_ctl ADD failed"<<std::endl;
        ch->SetInEpoll();
    }
}

std::vector<Channel*> Epoll::loop(int timeout)
{
    int events=epoll_wait(epfd_,revents_,maxEvents_,timeout);
    std::vector<Channel*> res;

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
        {
            Channel* ch=(Channel*)revents_[i].data.ptr;
            ch->SetRevents(revents_[i].events);
            res.push_back(ch);
        }
    }
    return res;
}

void Epoll::Del(Channel* ch)
{
    if(epoll_ctl(epfd_,EPOLL_CTL_DEL,ch->fd(),NULL)==-1)
        std::cout<<"epoll_ctl EDL failed"<<std::endl;
}