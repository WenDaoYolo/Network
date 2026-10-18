#include "RecvBuffer.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"

std::vector<Socket*> client_sockets;

void Loop()
{
    Socket server_socket(CreateSocket(0));
    server_socket.SetReuseAddr(true);
    server_socket.SetReusePort(true);

    InetAddress listenIA("172.20.10.2",2026);
    server_socket.bind(listenIA);
    server_socket.listen(10);
    
    Epoll ep;
    ep.AddFd(server_socket.fd(),EPOLLIN|EPOLLET);
    
    while(true)
    {
        std::cout<<"waiting events..."<<std::endl;
	    std::vector<epoll_event> events=ep.loop();

            for(int i=0;i<events.size();i++)
            {
                if(events[i].events&EPOLLIN)
                {
                    if(events[i].data.fd==server_socket.fd())
                    {
                        InetAddress clientIA;
                        Socket* client_socket=new Socket(server_socket.accept4(clientIA));
                        client_sockets.push_back(client_socket);

                        ep.AddFd(client_socket->fd(),EPOLLIN|EPOLLET);
                        std::cout<<"client "<<client_socket->fd()<<" connect"<<std::endl;
                        std::cout<<"IP:"<<clientIA.ip()<<" PORT:"<<clientIA.port()<<std::endl;
                    }
                    else
                    {
                        RecvBuffer buffer;

                        while(true)
                        {
                            size_t readables=buffer.CheckWritAble(1024);
                            int nums=recv(events[i].data.fd,buffer.WritePtr(),readables,0);

                            if(nums<0)
                            {
                                if(errno==EAGAIN) break;
                                std::cout<<"call recv failed"<<std::endl;
                            }
                            else if(nums==0)
                            {
                                ep.DelFd(events[i].data.fd);
                                close(events[i].data.fd);
                                std::cout<<"client"<<events[i].data.fd<<" disconnect"<<std::endl;
                                break;
                            }
                            else
                            {
                                buffer.MoveWritePtr(nums);
                            }
                        }

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
                                std::cout<<"client"<<events[i].data.fd<<":"<<msg<<std::endl;
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