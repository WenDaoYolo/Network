#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/poll.h>
#include<unistd.h>
#include<iostream>
#include<cstring>
#include<fcntl.h>

int main()
{
    int listen_socket=socket(PF_INET,SOCK_STREAM,0);
    sockaddr_in my_sa;
    my_sa.sin_family=PF_INET;
    my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
    my_sa.sin_port=htons(4399);

    bind(listen_socket,(sockaddr*)&my_sa,sizeof(my_sa));
    listen(listen_socket,5);
    
    while(true)
    {
        std::cout<<"listening..."<<std::endl;
        int fd=accept(listen_socket,NULL,NULL);
        std::cout<<"client "<<fd<<" is join"<<std::endl;
    }

    return 0;
}