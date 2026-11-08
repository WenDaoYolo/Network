#include "Buffer.h"

Buffer::Buffer(int incSize):space_(incSize),readIndex_(0),writeIndex_(0){}

size_t Buffer::ReadAbleBytes(){ return writeIndex_-readIndex_; }

const char* Buffer::ReadPtr(){ return &(space_[readIndex_]); }

char* Buffer::WritePtr(){ return &(space_[writeIndex_]); }

void Buffer::MoveWritePtr(size_t offset){ writeIndex_+=offset; }

void Buffer::MoveReadPrt(size_t offset)
{
    readIndex_+=offset;
    if(readIndex_==writeIndex_) readIndex_=writeIndex_=0;
}

size_t Buffer::CheckWritAble(size_t len)
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

Buffer::~Buffer(){};