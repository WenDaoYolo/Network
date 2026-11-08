#pragma once
#include<condition_variable>
#include<functional>
#include<iostream>
#include<iomanip>
#include<thread>
#include<vector>
#include<chrono>
#include<queue>
#include<mutex>

class CRPC_ThreadPool
{
	private:
		bool stop;
		std::mutex mtx;
		std::size_t ThreadNums;
		std::condition_variable cv;
		std::vector<std::thread> threads;
		std::queue<std::function<void()>> tasks;

	public:
        CRPC_ThreadPool(size_t ThreadNums);
        void enqueue(std::function<void()> task);
        ~CRPC_ThreadPool();
};