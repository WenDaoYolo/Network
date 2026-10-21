#include "RecvBuffer.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Accepter.h"

//...

void Loop()
{
    Epoll ep;
    Accepter acpt("172.20.10.2",2026,&ep);
    //...

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