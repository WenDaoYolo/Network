#pragma once
#include<condition_variable>
#include<functional>
#include<iostream>
#include<thread>
#include<mutex>
#include<queue>
#include<unistd.h>

class TaskQueue
{
    private:
        std::queue<std::function<void()>> tasks_;
        std::condition_variable pcv_,ccv_;
        std::mutex mtx_;
        int maxsize_;
        bool stop_;
        int eventfd_;
    public:
        TaskQueue(int maxsize);
        ~TaskQueue();
        void push(std::function<void()> task);
        std::vector<std::function<void()>> popAll();
        bool empty();
        void SetEventfd(int fd);
};