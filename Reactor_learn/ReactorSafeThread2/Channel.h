#pragma once
#include "Epoll.h"
#include "TaskQueue.h"
#include "Socket.h"
#include<functional>
#include<sys/eventfd.h>
#include<sys/timerfd.h>

class Epoll;
class TaskQueue;

class Channel
{
    private:
        int fd_=-1;
        Epoll* ep_=nullptr;
        bool inepoll_=false;
        uint32_t events_=0;
        uint32_t revents_=0;

        std::function<void()> readcallback_;
        std::function<void()> writecallback_;
        std::function<void()> errorcallback_;
    public:
        Channel(int fd,Epoll* ep);
        ~Channel();

        int fd();
        bool inepoll();
        uint32_t events();
        uint32_t revents();

        void SetET();
        void SetInEpoll();
        void EnableReading();
        bool IsWriting();
        void EnableWriting();
        void DisableWriting();
        void SetEvents(uint32_t ev);
        void SetRevents(uint32_t ev);

        void HandleEvent();
        void SetReadCallBack(std::function<void()> fun);
        void SetWriteCallBack(std::function<void()> fun);
        void SetErrorCallBack(std::function<void()> fun);

        std::function<void()> GetReadCallBack();
};

void SetWeakUpChannel(Epoll* ep,TaskQueue* taskq,Channel** wkupch);
void SetTimmerChannel(Epoll* ep,Channel** tmrch,int sec,int nsec,std::function<void()> fun);

