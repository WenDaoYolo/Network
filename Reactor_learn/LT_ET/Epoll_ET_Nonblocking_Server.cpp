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

	//将监听socket设为非阻塞IO
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
                        int client_fd=accept4(listen_fd,NULL,NULL,SOCK_NONBLOCK|SOCK_CLOEXEC);
                        if(client_fd==-1)
                        {
                            std::cout<<"accept4 failed"<<std::endl;
                            continue;
                        }

                        epoll_event cevent;
                        cevent.data.fd=client_fd;
                        cevent.events=EPOLLIN|EPOLLET;
                        epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd,&cevent);
                        std::cout<<"client "<<client_fd<<" connect"<<std::endl;
                    }
                    else
                    {
                        char buffer[1024]={0};
                        int target=0,total=0;
                        char* pos=buffer;

                        int len=recv(revents[i].data.fd,&target,sizeof(int),0);
                        
                        while(true)
                        {
                            int recv_once=recv(revents[i].data.fd,pos,target-total,0);

                            if(total==target)
                            {
                                std::cout<<"client"<<revents[i].data.fd<<":"<<buffer<<std::endl;
                                break;
                            }

                            if(len<0||recv_once<0)
                            {
                                if(errno!=EAGAIN)
                                {
                                    std::cout<<"call recv failed"<<std::endl;
                                }
                                break;
                            }
                            else if(len==0||recv_once==0)
                            {
                                epoll_ctl(epfd,EPOLL_CTL_DEL,revents[i].data.fd,NULL);
                                close(revents[i].data.fd);
                                std::cout<<"client"<<revents[i].data.fd<<" disconnect"<<std::endl;
                                break;
                            }
                            else
                            {
                                total+=recv_once;
                                pos+=recv_once;
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