#include<iostream>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>
#include "RecvBuffer.h"
#include "InetAddress.h"

void InitSocket(int& listen_fd)
{
    listen_fd=socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
    if(listen_fd==-1)
        throw std::runtime_error("create listen sokcet:");
    
    InetAddress listenIA("172.20.10.2",2026);
    bind(listen_fd,listenIA.addr(),sizeof(sockaddr));
    listen(listen_fd,5);
}

void Loop()
{
	int listen_fd;
    InitSocket(listen_fd);
    int epfd=epoll_create(1);
    epoll_event revents[64];
    epoll_event levent;
    //监听读事件并采用边缘触发
    levent.events=EPOLLIN|EPOLLET;
    levent.data.fd=listen_fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,listen_fd,&levent);
    
    while(true)
    {
        std::cout<<"waiting events..."<<std::endl;
	    int events=epoll_wait(epfd,revents,64,-1);

        if(events<0)
        {
            if(errno==EINTR) continue;
            std::cout<<"call epoll_wait failed"<<std::endl;
        }
        else if(events==0)
        {
            std::cout<<"no event"<<std::endl;
        }
        else
        {
            for(int i=0;i<events;i++)
            {
                if(revents[i].events&EPOLLIN)
                {
                    if(revents[i].data.fd==listen_fd)
                    {
                        sockaddr_in csai;
                        socklen_t len=sizeof(csai);

                        int client_fd=accept4(listen_fd,(sockaddr*)&csai,&len,SOCK_NONBLOCK|SOCK_CLOEXEC);
                        if(client_fd==-1)
                        {
                            std::cout<<"accept4 failed"<<std::endl;
                            continue;
                        }

                        InetAddress clientIA(csai);

                        epoll_event cevent;
                        cevent.data.fd=client_fd;
                        cevent.events=EPOLLIN|EPOLLET;
                        epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&cevent);
                        
                        std::cout<<"client "<<client_fd<<" connect"<<std::endl;
                        std::cout<<"IP:"<<clientIA.ip()<<" PORT:"<<clientIA.port()<<std::endl;
                    }
                    else
                    {
                        RecvBuffer buffer;

                        //read
                        while(true)
                        {
                            size_t readables=buffer.CheckWritAble(1024);
                            int nums=recv(revents[i].data.fd,buffer.WritePtr(),readables,0);

                            if(nums<0)
                            {
                                if(errno==EAGAIN) break;
                                std::cout<<"call recv failed"<<std::endl;
                            }
                            else if(nums==0)
                            {
                                epoll_ctl(epfd,EPOLL_CTL_DEL,revents[i].data.fd,NULL);
                                close(revents[i].data.fd);
                                std::cout<<"client"<<revents[i].data.fd<<" disconnect"<<std::endl;
                                break;
                            }
                            else
                            {
                                buffer.MoveWritePtr(nums);
                            }
                        }

                        //scan
                        if(buffer.ReadAbleBytes()>=4)
                        {
                            int len=0;
                            memcpy(&len,buffer.ReadPtr(),sizeof(int));
                            len=ntohl(len);
                            
                            if(buffer.ReadAbleBytes()>=4+len)
                            {
                                buffer.MoveReadPrt(4);
                                std::string msg(buffer.ReadPtr(),len);
                                buffer.MoveReadPrt(len);
                                std::cout<<"client"<<revents[i].data.fd<<":"<<msg<<std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}

int main()
{
	Loop();

	return 0;
}