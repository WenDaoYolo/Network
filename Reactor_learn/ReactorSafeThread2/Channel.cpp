#include "Channel.h"

Channel::Channel(int fd,Epoll* ep):fd_(fd),ep_(ep){}

Channel::~Channel(){}

int Channel::fd(){ return fd_; }
bool Channel::inepoll(){ return inepoll_; }
uint32_t Channel::events(){ return events_; }
uint32_t Channel::revents(){ return revents_; }

void Channel::SetET(){ events_|=EPOLLET; }

void Channel::SetInEpoll(){ inepoll_=true; }

void Channel::EnableReading(){ events_|=EPOLLIN; }

void Channel::EnableWriting(){ events_|=EPOLLOUT; }

bool Channel::IsWriting(){ return events_&EPOLLOUT; }

void Channel::DisableWriting(){ events_ &=~EPOLLOUT; }

void Channel::SetEvents(uint32_t ev){ events_=ev; }

void Channel::SetRevents(uint32_t ev){ revents_=ev; }

void Channel::SetReadCallBack(std::function<void()> fun){ readcallback_=fun; }
void Channel::SetWriteCallBack(std::function<void()> fun){ writecallback_=fun; }
void Channel::SetErrorCallBack(std::function<void()> fun){ errorcallback_=fun; }

void Channel::HandleEvent()
{   
    if((revents_&EPOLLERR)||(revents_&EPOLLHUP))
    {
        if(errorcallback_) errorcallback_();
    }
    if(revents_&EPOLLIN)
    {
        if(readcallback_) readcallback_();
    }
    if(revents_&EPOLLOUT)
    {
        if(writecallback_) writecallback_();
    }
}

void SetWeakUpChannel(Epoll* ep,TaskQueue* taskq,Channel** wkupch)
{
    int fd=eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if(fd==-1)
    {
        std::cout<<"eventfd() error!"<<std::endl;
        return;
    }

    *wkupch=new Channel(fd,ep);
    (*wkupch)->EnableReading();
    (*wkupch)->SetReadCallBack
    (
        [taskq,wkupch]()
        {
            uint64_t nn;
            read((*wkupch)->fd(),&nn,sizeof(nn));

            std::vector<std::function<void()>> rtasks;
            if(!taskq->empty())
                rtasks=taskq->popAll();
            for(auto& task:rtasks)
                if(task)
                {
                    std::cout<<"main thread get task!"<<std::endl;
                    task();
                }
        }
    );
    ep->Update(*wkupch);
}

std::function<void()> Channel::GetReadCallBack(){ return readcallback_; }

void SetTimmerChannel(Epoll* ep,Channel** tmrch,int sec,int nsec,std::function<void()> fun)
{
    int fd=timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);
    if(fd==-1)
    {
        std::cout<<"timerfd_create error!"<<std::endl;
        return;
    }

    struct itimerspec new_value;
    new_value.it_value.tv_sec=sec;
    new_value.it_value.tv_nsec=nsec;
    new_value.it_interval.tv_sec=sec;
    new_value.it_interval.tv_nsec=nsec;
    timerfd_settime(fd, 0, &new_value, NULL);

    *tmrch=new Channel(fd,ep);
    (*tmrch)->EnableReading();

    (*tmrch)->SetReadCallBack
    (
        [fd,fun]()
        {
            uint64_t nn;
            read(fd,&nn,sizeof(nn));

            if(fun) fun();
        }   
    );
    ep->Update(*tmrch);
}