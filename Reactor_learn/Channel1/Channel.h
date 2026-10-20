#pragma once
#include<sys/epoll.h>

class Epoll;

class Channel
{
    private:
        int fd_=-1;
        Epoll* ep_=nullptr;
        bool inepoll_=false;
        uint32_t events_=0;
        uint32_t revents_=0;
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
        void EnableWriting();
        void SetEvents(uint32_t ev);
        void SetRevents(uint32_t ev);
};