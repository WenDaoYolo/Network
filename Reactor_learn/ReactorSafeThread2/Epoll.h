#pragma once
#include<sys/epoll.h>
#include<unistd.h>
#include<vector>
#include<iostream>
#include "Channel.h"

class Channel;                                           //前向声明

class Epoll
{
    private:
        int epfd_=-1;                                    //epollfd句柄
        static const int maxEvents_=256;                 //事件的最大数
        epoll_event revents_[maxEvents_];                //存储发生的事件
    public:
        Epoll();                                         //构造函数(构造epollfd)
        ~Epoll();                                        //析构函数(关闭epollfd)

        void Del(Channel* ch);                           //将Channel及其socket从红黑树移除
        void Update(Channel* ch);                        //将Channel及其socket添加到红黑树
        std::vector<Channel*> loop(int timeout=-1);      //监听并获取发生事件的Channel指针集合
};