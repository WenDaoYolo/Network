#include "Channel.h"

Channel::Channel(int fd,Epoll* ep):fd_(fd),ep_(ep){}

Channel::~Channel(){}

int Channel::fd(){ return fd_; }
bool Channel::inepoll(){ return inepoll_; }
uint32_t Channel::events(){ return events_; }
uint32_t Channel::revents(){ return revents_; }

void Channel::SetET(){ events_|=EPOLLET; }

void Channel::SetInEpoll(){ inepoll_=true; }

void Channel::EnableReading(){ events_|=EPOLLIN; }

void Channel::EnableWriting(){ events_|=EPOLLOUT; }

void Channel::SetEvents(uint32_t ev){ events_=ev; }

void Channel::SetRevents(uint32_t ev){ revents_=ev; }