#include "CRPC_ThreadPool.h"

CRPC_ThreadPool::CRPC_ThreadPool(size_t ThreadNums):stop(false),ThreadNums(ThreadNums)
{
    for(int i=0;i<ThreadNums;i++)
    {
        std::thread t([this]
        {
            while(true)
            {
                std::unique_lock<std::mutex> lock(this->mtx);
                cv.wait(lock,[this]{return !(this->tasks).empty()||this->stop;});
                if(this->stop&&tasks.empty()) return;

                std::function<void()> task=tasks.front();
                tasks.pop();
                std::cout<<"GetTask!"<<std::endl;//测试专用，使用时可注释掉
                lock.unlock();//取到任务后释放锁资源，也可控制锁的粒度自动释放
                task();
            }
        });
    threads.push_back(move(t));
    }	
}
		
void CRPC_ThreadPool::enqueue(std::function<void()> task)
{
    std::unique_lock<std::mutex> lock(this->mtx);
    this->tasks.push(task);
    cv.notify_one();
}
		
CRPC_ThreadPool::~CRPC_ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        this->stop=true;
    }
    cv.notify_all();

    for(int i=0;i<this->ThreadNums;i++)
        this->threads[i].join();
}