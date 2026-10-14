#include<iostream>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>

void InitSocket(int& listen_fd)
{
    listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd==-1)
        throw std::runtime_error("create listen sokcet:");
    
    sockaddr_in sai;
    sai.sin_family=AF_INET;
    sai.sin_port=htons(2026);
    sai.sin_addr.s_addr=htonl(INADDR_ANY);

    bind(listen_fd,(sockaddr*)&sai,sizeof(sai));
    listen(listen_fd,5);
}

void Loop()
{
    int epfd=epoll_create(1);

    int listen_fd;
    InitSocket(listen_fd);

    epoll_event levent;
    levent.events=EPOLLIN;
    levent.data.fd=listen_fd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,listen_fd,&levent);
    
    while(true)
    {
        epoll_event revents[64];
        std::cout<<"waiting events..."<<std::endl;
        int events=epoll_wait(epfd,revents,64,-1);

        if(events<0)
        {
            if(errno==EINTR)
                continue;
            else
                std::cout<<"call epoll_wait failed!"<<std::endl;
        }
        else if(events==0)
        {
            std::cout<<"no event..."<<std::endl;
        }
        else
        {
            for(int i=0;i<events;i++)
            {
                if(revents[i].events&EPOLLIN)
                {
                    if(revents[i].data.fd==listen_fd)
                    {
                        int client_fd=accept(listen_fd,NULL,NULL);
                        if(client_fd==-1)
                        {
                            std::cout<<"accept failed!"<<std::endl;
                            continue;
                        }

                        epoll_event client_event;
                        client_event.events=EPOLLIN;
                        client_event.data.fd=client_fd;
                        epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&client_event);
                        std::cout<<"client"<<client_fd<<" connect"<<std::endl;
                    }
                    else
                    {
                        char buffer[64];
                        int nums=recv(revents[i].data.fd,buffer,sizeof(buffer),0);

                        if(nums<0)
                            std::cout<<"recv failed!"<<std::endl;
                        else if(nums==0)
                        {
                            epoll_ctl(epfd,EPOLL_CTL_DEL,revents[i].data.fd,NULL);
                            close(revents[i].data.fd);
                            std::cout<<"client"<<revents[i].data.fd<<" disconnect"<<std::endl;
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