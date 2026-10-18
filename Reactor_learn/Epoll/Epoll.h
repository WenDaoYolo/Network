#pragma once
#include<sys/epoll.h>
#include<unistd.h>
#include<vector>
#include<iostream>

class Epoll
{
    private:
        int epfd_=-1;
        static const int maxEvents_=256;
        epoll_event revents_[maxEvents_];
    public:
        Epoll();
        ~Epoll();

        void DelFd(int fd);
        void AddFd(int fd,int op);
        std::vector<epoll_event> loop(int timeout=-1);
};
