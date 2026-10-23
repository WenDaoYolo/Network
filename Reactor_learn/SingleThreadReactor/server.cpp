#include "TcpConnection.h"
#include "Accepter.h"
#include<unordered_map>

std::unordered_map<int,TcpConnection*> g_maps;
Epoll g_ep;

void Loop()
{
    Accepter acpt("172.20.10.2",2026,&g_ep);
    acpt.SetNewConnectionCallBack
    (
        [&](int client_fd,InetAddress IA)
        {
            TcpConnection* tmp=new TcpConnection(client_fd,&g_ep);
            g_maps[client_fd]=tmp;

            tmp->SetCloseCallBack
            (
                [&](int close_fd)
                {
                    TcpConnection* del=g_maps[close_fd];
                    g_maps.erase(close_fd);
                    delete del;//
                    std::cout<<"client"<<close_fd<<" disconnect"<<std::endl;
                }
            );
            std::cout<<"client "<<client_fd<<" connect"<<std::endl;
            std::cout<<"IP:"<<IA.ip()<<" PORT:"<<IA.port()<<std::endl;
        }
    );

    while(true)
    {
        std::cout<<"waiting events..."<<std::endl;
	    std::vector<Channel*> channels=g_ep.loop();

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