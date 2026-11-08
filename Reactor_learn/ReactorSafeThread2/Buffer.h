#pragma once
#include<iostream>
#include<memory.h>
#include<vector>

class Buffer
{
    private:
        std::vector<char> space_;
        size_t readIndex_,writeIndex_;
    public:
        Buffer(int incSize=1024);
        ~Buffer();

        char* WritePtr();
        const char* ReadPtr();
        size_t ReadAbleBytes();
        size_t CheckWritAble(size_t len);

        void MoveReadPrt(size_t offset);
        void MoveWritePtr(size_t offset);
};