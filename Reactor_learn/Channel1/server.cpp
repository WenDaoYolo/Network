#include "RecvBuffer.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"

std::vector<Socket*> client_sockets;
std::vector<Channel*> client_channels;

void Loop()
{
    Socket server_socket(CreateSocket(0));
    server_socket.SetReuseAddr(true);
    server_socket.SetReusePort(true);

    InetAddress listenIA("172.20.10.2",2026);
    server_socket.bind(listenIA);
    server_socket.listen(10);
    
    Epoll ep;
    Channel server_ch(server_socket.fd(),&ep);
    server_ch.EnableReading();
    server_ch.SetET();

    ep.Update(&server_ch);
    
    while(true)
    {
        std::cout<<"waiting events..."<<std::endl;
	    std::vector<Channel*> channels=ep.loop();

        for(int i=0;i<channels.size();i++)
        {
            if(channels[i]->revents()&EPOLLIN)
            {
                if(channels[i]->fd()==server_socket.fd())
                {
                    InetAddress clientIA;
                    Socket* client_socket=new Socket(server_socket.accept4(clientIA));
                    Channel* client_ch=new Channel(client_socket->fd(),&ep);
                    client_ch->EnableReading();
                    client_ch->SetET();

                    client_sockets.push_back(client_socket);
                    client_channels.push_back(client_ch);
                    ep.Update(client_ch);

                    std::cout<<"client "<<client_socket->fd()<<" connect"<<std::endl;
                    std::cout<<"IP:"<<clientIA.ip()<<" PORT:"<<clientIA.port()<<std::endl;
                }
                else
                {
                    RecvBuffer buffer;

                    while(true)
                    {
                        size_t readables=buffer.CheckWritAble(1024);
                        int nums=recv(channels[i]->fd(),buffer.WritePtr(),readables,0);

                        if(nums<0)
                        {
                            if(errno==EAGAIN) break;
                            std::cout<<"call recv failed"<<std::endl;
                        }
                        else if(nums==0)
                        {
                            ep.Del(channels[i]);
                            close(channels[i]->fd());
                            std::cout<<"client"<<channels[i]->fd()<<" disconnect"<<std::endl;
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
                            std::cout<<"client"<<channels[i]->fd()<<":"<<msg<<std::endl;
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

    for(int i=0;i<client_channels.size();i++)
    {
        if(client_channels[i]!=nullptr)
            delete client_channels[i];
    }
}

int main()
{
	Loop();
    Destroy();

	return 0;
}