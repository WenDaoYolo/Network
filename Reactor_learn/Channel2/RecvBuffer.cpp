#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int incSize):space_(incSize),readIndex_(0),writeIndex_(0){}

size_t RecvBuffer::ReadAbleBytes(){ return writeIndex_-readIndex_; }

const char* RecvBuffer::ReadPtr(){ return &(space_[readIndex_]); }

char* RecvBuffer::WritePtr(){ return &(space_[writeIndex_]); }

void RecvBuffer::MoveWritePtr(size_t offset){ writeIndex_+=offset; }

void RecvBuffer::MoveReadPrt(size_t offset)
{
    readIndex_+=offset;
    if(readIndex_==writeIndex_) readIndex_=writeIndex_=0;
}

size_t RecvBuffer::CheckWritAble(size_t len)
{
    if(space_.size()-writeIndex_<len)
    {
        memmove(&space_[0],ReadPtr(),ReadAbleBytes());
        writeIndex_-=readIndex_;
        readIndex_=0;

        if(space_.size()-writeIndex_<len)
            space_.resize(space_.size()+len*2);
    }
    return space_.size()-writeIndex_;
}

RecvBuffer::~RecvBuffer(){};