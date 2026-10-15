#include<iostream>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>

void setnblocking(int& fd)
{
    int state=fcntl(fd,F_GETFL,NULL);
    if(state==-1) state=0;
    fcntl(fd,F_SETFL,state|O_NONBLOCK);
}

void InitSocket(int& listen_fd)
{
    listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd==-1)
        throw std::runtime_error("create listen sokcet:");
    
    sockaddr_in sai;
    sai.sin_family=AF_INET;
    sai.sin_port=htons(2026);
    sai.sin_addr.s_addr=htonl(INADDR_ANY);

    setnblocking(listen_fd);
    bind(listen_fd,(sockaddr*)&sai,sizeof(sai));
    listen(listen_fd,5);
}

void Loop()
{
    int listen_fd;
    InitSocket(listen_fd);
    int epfd=epoll_create(1);
    epoll_event revents[64];
    epoll_event levent;
    levent.events=EPOLLIN;
    levent.data.fd=listen_fd;

    epoll_ctl(epfd,EPOLL_CTL_ADD,listen_fd,&levent);
    while(true)
    {
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
                        int client_fd=accept4(listen_fd,NULL,NULL,SOCK_CLOEXEC|SOCK_NONBLOCK);
                        if(client_fd<0)
                        {
                            if(errno==EAGAIN) continue;
                            std::cout<<"call accept4 failed"<<std::endl;
                        }
                        else
                        {
                            epoll_event cevents;
                            cevents.events=EPOLLIN;
                            cevents.data.fd=client_fd;
                            epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&cevents);
                            std::cout<<"client"<<client_fd<<" connect"<<std::endl;
                        }
                    }
                    else
                    {
                        char buffer[64];
                        int nums=recv(revents[i].data.fd,buffer,sizeof(buffer),0);
                        
                        if(nums<0)
                        {
                            if(errno==EAGAIN) continue;
                            std::cout<<"recv failed"<<std::endl;
                        }
                        else if(nums==0)
                        {
                            std::cout<<"client"<<revents[i].data.fd<<" disconnect"<<std::endl;
                            close(revents[i].data.fd);
                        }
                        else
                        {
                            std::cout<<"client"<<revents[i].data.fd<<":"<<buffer<<std::endl;
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