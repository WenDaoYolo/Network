#include "TcpConnection.h"
#include "TaskQueue.h"
#include "Accepter.h"
#include<unordered_map>
#include<memory>

std::unordered_map<int,std::shared_ptr<TcpConnection>> g_maps;
Epoll g_ep;
CRPC_ThreadPool g_tp(9);
TaskQueue g_taskq(100);
Channel* g_weakupch=nullptr;
Channel* g_timmerch1=nullptr;
Channel* g_timmerch2=nullptr;

void TimmerLog()
{
    std::cout<<"timmer is running!"<<std::endl;
}

void TimmerBroadCast()//定时广播
{
	for (auto& pair : g_maps) 
    {
        // 1. pair.first 是 fd, pair.second 是 shared_ptr<TcpConnection>
        // 2. 必须检查指针是否为空（防止并发 erase 时拿到空指针）
        if (pair.second != nullptr) 
        {
            pair.second->Register();
        }
    }
}

void TimmerClearSocket()//清理长时间无行为的客户端连接
{
	
}

void Loop()
{
    Accepter acpt("172.20.10.2",2026,&g_ep);

    SetWeakUpChannel(&g_ep,&g_taskq,&g_weakupch);
    g_taskq.SetEventfd(g_weakupch->fd());
    SetTimmerChannel(&g_ep,&g_timmerch1,5,0,TimmerLog);
    SetTimmerChannel(&g_ep,&g_timmerch2,10,0,TimmerBroadCast);

    acpt.SetNewConnectionCallBack
    (
        [&](int client_fd,InetAddress IA)
        {
            std::shared_ptr<TcpConnection> tmp(new TcpConnection(client_fd,&g_ep,&g_tp,&g_taskq));
            g_maps[client_fd]=tmp;

            tmp->SetCloseCallBack
            (
                [&](int close_fd)
                {
                    g_maps.erase(close_fd);
                    std::cout<<"client"<<close_fd<<" disconnect"<<std::endl;
                }
            );
            tmp->Init();
            std::cout<<"client "<<client_fd<<" connect"<<std::endl;
            std::cout<<"IP:"<<IA.ip()<<" PORT:"<<IA.port()<<std::endl;
        }
    );

    while(true)
    {
	    std::vector<Channel*> channels=g_ep.loop();

        for(int i=0;i<channels.size();i++)
        {
            channels[i]->HandleEvent();
        }
    }
}

void Destroy()
{
	//清理残留的普通客户端连接
	g_maps.clear();

	//fd通过Socket类析构自动关闭的，但是只有accepter和TcpConnection的fd是通过Socket
	//管理的，eventfd和timefd都是直接用channel管理的(内部只有fd)，所以需要手动清理

	//注意，这里动态创建的eventfd通道是用普通指针管理的，需要也需要手动回收内存
    if(g_weakupch!=nullptr)
    {
	    g_ep.Del(g_weakupch);//先将其从红黑树移除
	    close(g_weakupch->fd());
	    delete g_weakupch;
	    g_weakupch=nullptr;
    }

    if(g_timmerch1!=nullptr)
    {
        g_ep.Del(g_timmerch1);
        close(g_timmerch1->fd());
        delete g_timmerch1;
        g_timmerch1=nullptr;
    }

    if(g_timmerch2!=nullptr)
    {
        g_ep.Del(g_timmerch2);
        close(g_timmerch2->fd());
        delete g_timmerch2;
        g_timmerch2=nullptr;
    }
}

int main()
{
	Loop();
    Destroy();

	return 0;
}