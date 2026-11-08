#include "TcpConnection.h"
#include "Accepter.h"
#include<unordered_map>
#include<memory>

std::unordered_map<int,std::shared_ptr<TcpConnection>> g_maps;
Epoll g_ep;
CRPC_ThreadPool g_tp(9);

void Loop()
{
    Accepter acpt("172.20.10.2",2026,&g_ep);
    acpt.SetNewConnectionCallBack
    (
        [&](int client_fd,InetAddress IA)
        {
            std::shared_ptr<TcpConnection> tmp(new TcpConnection(client_fd,&g_ep,&g_tp));
            g_maps[client_fd]=tmp;

            tmp->SetCloseCallBack
            (
                [&](int close_fd)
                {
                    g_maps.erase(close_fd);
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