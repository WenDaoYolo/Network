#include<iostream>
#include<sys/socket.h>
#include<sys/select.h>
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

int FindNewMax(fd_set& fds,int old_max_fd)
{
    for(int i=old_max_fd-1;i>=0;i--)
    {
        if(FD_ISSET(i,&fds))
            return i;
    }
    return -1;
}

void Loop()
{
    fd_set fds;
    FD_ZERO(&fds);

    int listen_fd;
    InitSocket(listen_fd);

    int max_fd=listen_fd;
    FD_SET(listen_fd,&fds);

    while(true)
    {
        fd_set rfds=fds;
        std::cout<<"waiting event..."<<std::endl;
        int events=select(max_fd+1,&rfds,NULL,NULL,NULL);
        
        if(events==0)
        {
            std::cout<<"no event"<<std::endl;
        }
        else if(events<0)
        {
            std::cout<<"call select error!"<<std::endl;
            continue;
        }
        else
        {
            for(int i=0;i<=max_fd;i++)
            {
                if(FD_ISSET(i,&rfds))
                {
                    if(listen_fd==i)
                    {
                        int client_fd=accept(listen_fd,NULL,NULL);
                        if(client_fd==-1)
                        {
                            std::cout<<"accept failed!"<<std::endl;
                            continue;
                        }
                        
                        FD_SET(client_fd,&fds);
                        if(client_fd>max_fd) max_fd=client_fd;//socket is order alocate
                        std::cout<<"client"<<client_fd<<" connected"<<std::endl;
                    }
                    else
                    {
                        char buffer[64]={0};
                        int recv_once=recv(i,buffer,sizeof(buffer),0);
                        
                        if(recv_once>0)
                            std::cout<<"client"<<i<<":"<<buffer<<std::endl;
                        else if(recv_once<0)
                            std::cout<<"recv error!"<<std::endl;
                        else
                        {
                            FD_CLR(i,&fds);
                            if(i==max_fd)
                                max_fd=FindNewMax(fds,max_fd);
                            close(i);
                            std::cout<<"client"<<i<<" disconnect"<<std::endl;
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