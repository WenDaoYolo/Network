#include<iostream>
#include<sys/socket.h>
#include<sys/poll.h>
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

int FindNewMax(pollfd* polls,int old_max_fd)
{
    for(int i=old_max_fd-1;i>=0;i--)
    {
        if(polls[i].fd!=-1)
            return i;
    }
    return -1;
}

void InitPolls(pollfd* polls,int len)
{
    for(int i=0;i<len;i++)
    {
        polls[i].fd=-1;
        polls[i].events=0;
        polls[i].revents=0;
    }
}

void Loop()
{
    int listen_fd;
    pollfd polls[1024];
    InitPolls(polls,1024);
    InitSocket(listen_fd);

    int max_fd=listen_fd;
    polls[listen_fd].fd=listen_fd;
    polls[listen_fd].events=POLLIN;

    while(true)
    {
        std::cout<<"wainting events..."<<std::endl;
        int events=poll(polls,max_fd+1,-1);

        if(events==-1)
        {
            std::cout<<"call poll failed!"<<std::endl;
        }
        else if(events==0)
        {
            std::cout<<"no events..."<<std::endl;
        }
        else
        {
            for(int i=0;i<=max_fd;i++)
            {
                if(polls[i].fd==-1) continue;
                if(polls[i].revents==0) continue;

                if(polls[i].revents&POLLIN)
                {
                    if(polls[i].fd==listen_fd)
                    {
                        int client_fd=accept(listen_fd,NULL,NULL);
                        if(client_fd==-1)
                        {
                            std::cout<<"accept failed!"<<std::endl;
                            continue;
                        }

                        polls[client_fd].fd=client_fd;
                        polls[client_fd].events=POLLIN;
                        polls[client_fd].revents=0;
                        if(client_fd>max_fd) max_fd=client_fd;
                        std::cout<<"client"<<client_fd<<" connect"<<std::endl;
                    }
                    else
                    {
                        char buffer[64]={0};
                        int nums=recv(polls[i].fd,buffer,sizeof(buffer),0);
                        
                        if(nums<0)
                        {
                            std::cout<<"recv failed"<<std::endl;
                            continue;
                        }
                        else if(nums==0)
                        {
                            if(polls[i].fd==max_fd) max_fd=FindNewMax(polls,max_fd);
                            close(polls[i].fd);
                            
                            std::cout<<"client"<<polls[i].fd<<" disconnect"<<std::endl;
                            polls[i].events=0;
                            polls[i].fd=-1;
                        }
                        else
                        {
                            std::cout<<"client"<<polls[i].fd<<":"<<buffer<<std::endl;
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