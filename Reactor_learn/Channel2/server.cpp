#include "RecvBuffer.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"

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
    
    /*
        ...
    */

    while(true)
    {
        std::cout<<"waiting events..."<<std::endl;
	    std::vector<Channel*> channels=ep.loop();

        for(int i=0;i<channels.size();i++)
        {
            channels[i]->HandleEvent();
        }
    }
}

int main()
{
	Loop();

	return 0;
}