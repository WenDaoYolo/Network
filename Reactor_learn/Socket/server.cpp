#include<iostream>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>
#include "RecvBuffer.h"
#include "InetAddress.h"
#include "Socket.h"

std::vector<Socket*> client_sockets;

void Loop()
{
    Socket server_socket(CreateSocket(0));
    server_socket.SetReuseAddr(true);
    server_socket.SetReusePort(true);

    InetAddress listenIA("172.20.10.2",2026);
    server_socket.bind(listenIA);
    server_socket.listen(10);
    
    int epfd=epoll_create(1);
    epoll_event revents[64];
    epoll_event levent;
    //监听读事件并采用边缘触发
    levent.events=EPOLLIN|EPOLLET;
    levent.data.fd=server_socket.fd();
    epoll_ctl(epfd,EPOLL_CTL_ADD,server_socket.fd(),&levent);
    
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
                    if(revents[i].data.fd==server_socket.fd())
                    {
                        InetAddress clientIA;
                        Socket* client_socket=new Socket(server_socket.accept4(clientIA));
                        client_sockets.push_back(client_socket);

                        epoll_event cevent;
                        cevent.data.fd=client_socket->fd();
                        cevent.events=EPOLLIN|EPOLLET;
                        epoll_ctl(epfd,EPOLL_CTL_ADD,client_socket->fd(),&cevent);
                        
                        std::cout<<"client "<<client_socket->fd()<<" connect"<<std::endl;
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

void Destroy()
{
    for(int i=0;i<client_sockets.size();i++)
    {
        if(client_sockets[i]!=nullptr)
            delete client_sockets[i];
    }
}

int main()
{
	Loop();
    Destroy();

	return 0;
}