#include "TaskQueue.h"

TaskQueue::TaskQueue(int maxsize):maxsize_(maxsize),stop_(false)
{
    
}

void TaskQueue::SetEventfd(int fd)
{
    eventfd_=fd;
}

void TaskQueue::push(std::function<void()> task)
{
    std::unique_lock<std::mutex> ulock(mtx_);
    pcv_.wait(ulock,[&](){ return tasks_.size()<maxsize_||stop_; });
   
    if(stop_) return;

    tasks_.push(task);
    ccv_.notify_one();

    uint64_t nn=1;
    write(eventfd_,&nn,sizeof(nn));
}

std::vector<std::function<void()>> TaskQueue::popAll()
{
    std::unique_lock<std::mutex> ulock(mtx_);
    std::vector<std::function<void()>> ts;
    while(!tasks_.empty())
    {
        ts.push_back(tasks_.front());
        tasks_.pop();
    }

    pcv_.notify_all();
    return ts;
}

TaskQueue::~TaskQueue()
{
    {
        std::unique_lock<std::mutex> ulock(mtx_);
        stop_=true;
    }
    pcv_.notify_all();
    ccv_.notify_all();
}

bool TaskQueue::empty()
{
    return tasks_.empty();
}